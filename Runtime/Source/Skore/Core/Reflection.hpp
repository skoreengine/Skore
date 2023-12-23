// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "StringView.hpp"
#include "String.hpp"
#include "TypeID.hpp"
#include "Array.hpp"
#include "SharedPtr.hpp"
#include "HashMap.hpp"
#include "Span.hpp"

namespace Skore
{
	class TypeHandler;

	struct TypeInfo
	{
		TypeID TypeId{};
	};

	struct FieldInfo
	{
		TypeID   OwnerId{};
		bool     IsConst{};
		bool     IsPointer{};
		bool     IsReference{};
		TypeInfo TypeInfo{};
	};

	template<typename Type>
	TypeInfo MakeTypeInfo()
	{
		return TypeInfo{
			.TypeId = GetTypeID<Type>()
		};
	}

	template<typename Owner, typename Field>
	constexpr FieldInfo MakeFieldInfo()
	{
		return FieldInfo{
			.OwnerId = GetTypeID<Owner>(),
			.IsConst = false,
			.IsPointer  = false,
			.IsReference  = false,
			.TypeInfo = MakeTypeInfo<Field>()
		};
	}

	class SK_API AttributeHandler
	{
		typedef ConstCPtr(*FnGetValue)(AttributeHandler* handler);
	public:
		AttributeHandler(TypeID typeId, usize size);

		void SetFnGetValue(FnGetValue fnGetValue);
		ConstCPtr GetValue();
	private:
		FnGetValue m_FnGetValue;
		TypeID     m_TypeId{};
		usize      m_Size{};
	};

	//type handlers

	class SK_API ConstructorHandler
	{
	public:
		typedef void (*PlacementNewFn)(ConstructorHandler* handler, CPtr memory, CPtr* params);
		typedef CPtr (*NewInstanceFn)(ConstructorHandler* handler, Allocator* allocator, CPtr* params);

		void SetPlacementNewFn(PlacementNewFn placementNewFn);
		void SetNewInstanceFn(NewInstanceFn newInstanceFn);

		CPtr NewInstance(Allocator* allocator, CPtr* params);

	private:
		PlacementNewFn m_PlacementNewFn;
		NewInstanceFn  m_NewInstanceFn;
	};

	class SK_API FieldHandler
	{
		typedef FieldInfo (*FnGetFieldInfo)(const FieldHandler* fieldHandler);
		typedef CPtr  (*FnGetFieldPointer)(const FieldHandler* fieldHandler, CPtr instance);
		typedef void  (*FnCopyValueTo)(const FieldHandler* fieldHandler, ConstCPtr instance, CPtr value);
		typedef void  (*FnSetValue)(const FieldHandler* fieldHandler, CPtr instance, ConstCPtr value);
	public:
		explicit FieldHandler(const String& name);

		FieldInfo GetFieldInfo() const;
		CPtr GetFieldPointer(CPtr instance) const;
		void CopyValueTo(ConstCPtr instance, CPtr value);
		void SetValue(CPtr instance, ConstCPtr value);

		template<typename T>
		T& GetFieldAs(CPtr instance)
		{
			return *static_cast<T*>(GetFieldPointer(instance));
		}

		template<typename T>
		void SetValueAs(CPtr instance, const T& value)
		{
			SetValue(instance, &value);
		}

		void SetFnGetFieldInfo(FnGetFieldInfo fieldInfoFn);
		void SetFnGetFieldPointer(FnGetFieldPointer fnGetFieldPointer);
		void SetFnCopyValueTo(FnCopyValueTo fnCopyValueTo);
		void SetFnSetValue(FnSetValue fnSetValue);
	private:
		String            m_Name;
		FnGetFieldInfo    m_FnGetFieldInfo;
		FnGetFieldPointer m_FnGetFieldPointer;
		FnCopyValueTo     m_FnCopyValueTo;
		FnSetValue        m_FnSetValue;

		HashMap<TypeID, SharedPtr<AttributeHandler>> m_Attributes{};
	};

	class SK_API TypeHandler
	{
		typedef void (*DestroyFn)(const TypeHandler* typeHandler, Allocator* allocator, CPtr instance);
	public:
		TypeHandler(const StringView& name, u32 version);
		void SetDestroyFn(DestroyFn destroyFn);

		ConstructorHandler& NewConstructor(TypeID* ids, usize size);
		ConstructorHandler* FindConstructor(TypeID* ids, usize size) const;
		Span<ConstructorHandler*> GetConstructors() const;

		FieldHandler& NewField(const StringView& fieldName);
		FieldHandler* FindField(const StringView& fieldName) const;
		Span<FieldHandler*> GetFields() const;


		CPtr NewInstance(Allocator* allocator = GetDefaultAllocator()) const
		{
			if (ConstructorHandler* constructor = FindConstructor(nullptr, 0))
			{
				return constructor->NewInstance(allocator, nullptr);
			}
			return nullptr;
		}

		template<typename ...Args>
		CPtr NewInstance(Allocator* allocator, Args&& ...args) const
		{
			TypeID ids[] = {GetTypeID<Args>()...,};

			if (ConstructorHandler* constructor = FindConstructor(ids, sizeof...(args)))
			{
				CPtr params[] = {&args...};
				return constructor->NewInstance(allocator, params);
			}
			return nullptr;
		}

		template<typename ...Args>
		CPtr NewInstance(Args&& ...args) const
		{
			return NewInstance(GetDefaultAllocator(), Traits::Forward<Args>(args)...);
		}

		void Destroy(CPtr instance, Allocator* allocator = GetDefaultAllocator()) const;

	private:
		String    m_Name{};
		u32       m_Version{};
		DestroyFn m_DestroyFn{};

		HashMap<usize, SharedPtr<ConstructorHandler>> m_Constructors;
		Array<ConstructorHandler*>                    m_ConstructorArray;

		HashMap<String, SharedPtr<FieldHandler>> m_Fields;
		Array<FieldHandler*>                     m_FieldArray;
	};

	//native impl

	template<typename Owner, typename ...Args>
	class NativeConstructorHandler
	{
	public:
		explicit NativeConstructorHandler(ConstructorHandler& constructorHandler) : m_ConstructorHandler(constructorHandler)
		{
			constructorHandler.SetNewInstanceFn(&NewInstanceImpl);
			constructorHandler.SetPlacementNewFn(PlacementNewImpl);
		}

	private:
		ConstructorHandler& m_ConstructorHandler;

		template<typename ...Vals>
		static void Eval(CPtr memory, CPtr* params, Traits::IndexSequence<>, Vals&& ...vals)
		{
			if constexpr (Traits::IsAggregate<Owner>)
			{
				new(PlaceHolder(), memory) Owner{Traits::Forward<Vals>(vals)...};
			}
			else
			{
				new(PlaceHolder(), memory) Owner(Traits::Forward<Vals>(vals)...);
			}
		}

		template<typename T, typename ...Tp, usize I, usize... Is, typename ...Vls>
		static void Eval(CPtr memory, CPtr* params, Traits::IndexSequence<I, Is...> seq, Vls&& ...vls)
		{
			return Eval<Tp...>(memory, params, Traits::IndexSequence<Is...>(), Traits::Forward<Vls>(vls)..., *static_cast<T*>(params[I]));
		}

		static void PlacementNewImpl(ConstructorHandler* handler, CPtr memory, CPtr* params)
		{
			const usize size = sizeof...(Args);
			Eval<Args...>(memory, params, Traits::MakeIntegerSequence<usize, size>{});
		}

		static CPtr NewInstanceImpl(ConstructorHandler* handler, Allocator* allocator, CPtr* params)
		{
			CPtr ptr = allocator->MemAlloc(allocator->Alloc, sizeof(Owner));
			PlacementNewImpl(handler, ptr, params);
			return ptr;
		}
	};


	template<typename Type, typename Enabler = void>
	struct NativeTypeHandlerFuncs
	{
		static void DestroyImpl(const TypeHandler* typeHandler, Allocator* allocator, CPtr instance){};
	};

	template<typename Type>
	struct NativeTypeHandlerFuncs<Type, Traits::VoidType<decltype(sizeof(Type) != 0)>>
	{
		static void DestroyImpl(const TypeHandler* typeHandler, Allocator* allocator, CPtr instance)
		{
			if constexpr (Traits::IsDestructible<Type>)
			{
				static_cast<Type*>(instance)->~Type();
			}
			allocator->MemFree(allocator->Alloc, instance, sizeof(Type));
		}
	};

	template<auto mfp, typename Owner, typename Field>
	class NativeFieldHandler
	{
	public:
		NativeFieldHandler(FieldHandler& fieldHandler) : m_fieldHandler(fieldHandler)
		{
			fieldHandler.SetFnGetFieldInfo(&GetFieldImpl);
			fieldHandler.SetFnGetFieldPointer(&FnGetFieldPointerImpl);
			fieldHandler.SetFnCopyValueTo(&CopyValueToImpl);
			fieldHandler.SetFnSetValue(&SetValue);
		}

	private:
		FieldHandler& m_fieldHandler;

		static FieldInfo GetFieldImpl(const FieldHandler* fieldHandler)
		{
			return MakeFieldInfo<Owner, Field>();
		}

		static CPtr FnGetFieldPointerImpl(const FieldHandler* fieldHandler, CPtr instance)
		{
			return &((*static_cast<Owner*>(instance)).*mfp);
		}

		static void CopyValueToImpl(const FieldHandler* fieldHandler, ConstCPtr instance, CPtr value)
		{
			*static_cast<Field*>(value) = ((*static_cast<const Owner*>(instance)).*mfp);
		}

		static void SetValue(const FieldHandler* fieldHandler, CPtr instance, ConstCPtr value)
		{
			((*static_cast<Owner*>(instance)).*mfp) = *static_cast<const Field*>(value);
		}
	};

	template<auto mfp, typename Field>
	struct FieldTemplateDecomposer
	{
	};

	template<auto mfp, typename Owner, typename FieldType>
	struct FieldTemplateDecomposer<mfp, FieldType Owner::*>
	{

		static TypeID GetTypeId()
		{
			return GetTypeID<FieldType>();
		}

		static auto CreateHandler(FieldHandler& fieldHandler)
		{
			return NativeFieldHandler<mfp, Owner, FieldType>(fieldHandler);
		}
	};

	template<typename Type>
	class NativeTypeHandler
	{
	public:
		explicit NativeTypeHandler(TypeHandler& typeHandler) : m_typeHandler(typeHandler)
		{
			if constexpr (Traits::IsDirectConstructible<Type>)
			{
				this->Constructor();
			}
			typeHandler.SetDestroyFn(&NativeTypeHandlerFuncs<Type>::DestroyImpl);
		}

		inline auto Constructor()
		{
			return NativeConstructorHandler<Type>(m_typeHandler.NewConstructor(nullptr, 0));
		}

		template<typename ...Args>
		inline auto Constructor()
		{
			TypeID ids[] = {GetTypeID<Args>()...,};
			return NativeConstructorHandler<Type, Args...>(m_typeHandler.NewConstructor(ids, sizeof...(Args)));
		}

		template<auto mfp>
		inline auto Field(const StringView& name)
		{
			using FieldDecomp = FieldTemplateDecomposer<mfp, decltype(mfp)>;
			return FieldDecomp::CreateHandler(m_typeHandler.NewField(name));
		}

	private:
		TypeHandler& m_typeHandler;
	};

	namespace Reflection
	{

		SK_API void Shutdown();

		SK_API TypeHandler& NewType(const StringView& name, const TypeInfo& typeInfo);
		SK_API TypeHandler* FindTypeByName(const StringView& name);

		template<typename T>
		SK_API decltype(auto) Type()
		{
			return NativeTypeHandler<T>(NewType(GetTypeName<T>(), MakeTypeInfo<T>()));
		}
	}
}