// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "StringView.hpp"
#include "String.hpp"
#include "TypeInfo.hpp"
#include "Array.hpp"
#include "SharedPtr.hpp"
#include "HashMap.hpp"
#include "Span.hpp"

namespace Skore
{

	class TypeHandler;

	struct FieldInfo
	{
		TypeID   OwnerId{};
		bool     IsConst{};
		bool     IsPointer{};
		bool     IsReference{};
		TypeInfo TypeInfo{};
	};

	template<typename Owner, typename Field>
	constexpr FieldInfo MakeFieldInfo()
	{
		return FieldInfo{
			.OwnerId = GetTypeID<Owner>(),
			.IsConst = false,
			.IsPointer  = false,
			.IsReference  = false,
			.TypeInfo = GetTypeInfo<Field>()
		};
	}

	struct FunctionHandlerCreation
	{
		StringView            Name{};
		TypeID                FunctionId{};
		Span<const FieldInfo> Params{};
		FieldInfo             Return{};
	};

	//type handlers

	class SK_API AttributeHandler
	{
		typedef ConstCPtr(*FnGetValue)(const AttributeHandler* handler);
	public:
		void SetFnGetValue(FnGetValue fnGetValue);
		ConstCPtr GetValue();
	private:
		FnGetValue m_FnGetValue;
	};

	class SK_API ParamHandler
	{
	public:
		ParamHandler(usize index, const FieldInfo& fieldInfo);
	private:
		FieldInfo m_FieldInfo{};
		String m_Name{};
	};

	class SK_API ConstructorHandler
	{
	public:
		typedef void (*PlacementNewFn)(ConstructorHandler* handler, CPtr memory, CPtr* params);
		typedef CPtr (*NewInstanceFn)(ConstructorHandler* handler, Allocator* allocator, CPtr* params);

		void SetPlacementNewFn(PlacementNewFn placementNewFn);
		void SetNewInstanceFn(NewInstanceFn newInstanceFn);

		CPtr NewInstance(Allocator* allocator, CPtr* params);
		void Construct(CPtr memory, CPtr* params);

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

	class SK_API FunctionHandler
	{
		typedef void(*FnCall)(const FunctionHandler* handler, CPtr instance, CPtr ret, CPtr* params);
	public:
		FunctionHandler(const FunctionHandlerCreation& creation);

		StringView GetName() const;
		Span<ParamHandler> GetParams() const;
		FieldInfo GetReturn() const;

		void SetFnCall(FnCall fnCall);
		void SetFunctionPointer(CPtr functionPointer);

		void Call(CPtr instance, CPtr ret, CPtr* params) const;

		CPtr GetFunctionPointer() const;

		AttributeHandler& NewAttribute(TypeID attributeId);
		ConstCPtr GetAttribute(TypeID attributeId) const;

		template<typename AttType>
		const AttType* GetAttribute() const
		{
			return static_cast<const AttType*>(GetAttribute(GetTypeID<AttType>()));
		}

	private:
		String              m_Name{};
		String              m_SimpleName{};
		TypeID              m_FunctionId{};
		Array<ParamHandler> m_Params{};
		FieldInfo           m_Return{};
		FnCall              m_FnCall{};
		CPtr                m_FunctionPointer{};

		HashMap<TypeID, SharedPtr<AttributeHandler>> m_Attributes;
		Array<AttributeHandler*>                     m_AttributeArray;
	};

	class SK_API TypeHandler
	{
		typedef void (*FnDestroy)(const TypeHandler* typeHandler, Allocator* allocator, CPtr instance);
		typedef void (*FnDestructor)(const TypeHandler* typeHandler, CPtr instance);
		typedef void (*FnCopy)(const TypeHandler* typeHandler, ConstCPtr source, CPtr dest);
	public:
		TypeHandler(const StringView& name, const TypeInfo& typeInfo, u32 version);
		void SetFnDestroy(FnDestroy fnDestroy);
		void SetFnCopy(FnCopy fnCopy);
		void SetFnDestructor(FnDestructor destructor);

		ConstructorHandler& NewConstructor(TypeID* ids, usize size);
		ConstructorHandler* FindConstructor(TypeID* ids, usize size) const;
		Span<ConstructorHandler*> GetConstructors() const;

		FieldHandler& NewField(const StringView& fieldName);
		FieldHandler* FindField(const StringView& fieldName) const;
		Span<FieldHandler*> GetFields() const;

		FunctionHandler& NewFunction(const FunctionHandlerCreation& creation);
		FunctionHandler* FindFunction(const StringView& functionName) const;
		Span<FunctionHandler*> GetFunctions() const;

		AttributeHandler& NewAttribute(TypeID attributeId);
		ConstCPtr GetAttribute(TypeID attributeId) const;


		void Construct(CPtr memory)
		{
			if (ConstructorHandler* constructor = FindConstructor(nullptr, 0))
			{
				return constructor->Construct(memory, nullptr);
			}
		}

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

		template<typename AttType>
		const AttType* GetAttribute() const
		{
			return static_cast<const AttType*>(GetAttribute(GetTypeID<AttType>()));
		}

		void Destroy(CPtr instance, Allocator* allocator = GetDefaultAllocator()) const;
		void Destructor(CPtr instance) const;
		void Copy(ConstCPtr source, CPtr dest) const;

		StringView GetName() const;
		const TypeInfo& GetTypeInfo() const;
		u32 GetVersion() const;

	private:
		String       m_Name{};
		TypeInfo     m_TypeInfo{};
		u32          m_Version{};
		FnDestroy    m_FnDestroy{};
		FnCopy       m_FnCopy{};
		FnDestructor m_FnDestructor{};

		HashMap<usize, SharedPtr<ConstructorHandler>> m_Constructors;
		Array<ConstructorHandler*>                    m_ConstructorArray;
		HashMap<String, SharedPtr<FieldHandler>>      m_Fields;
		Array<FieldHandler*>                          m_FieldArray;
		HashMap<String, SharedPtr<FunctionHandler>>   m_Functions;
		Array<FunctionHandler*>                       m_FunctionArray;
		HashMap<TypeID, SharedPtr<AttributeHandler>>  m_Attributes;
		Array<AttributeHandler*>                      m_AttributeArray;
	};

	//native impl

	template<typename Owner, typename Type>
	class NativeAttributeHandler
	{
	public:
		inline static Type Value{};

		NativeAttributeHandler(AttributeHandler& attributeHandler) : m_AttributeHandler(attributeHandler)
		{
			attributeHandler.SetFnGetValue(&GetValueImpl);
		}

	private:
		AttributeHandler& m_AttributeHandler;

		static ConstCPtr GetValueImpl(const AttributeHandler* handler)
		{
			return &Value;
		}
	};


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
		static void CopyImpl(const TypeHandler* typeHandler, ConstCPtr source, CPtr dest) {};
		static void DestructorImpl(const TypeHandler* typeHandler, CPtr instance){};
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

		static void CopyImpl(const TypeHandler* typeHandler, ConstCPtr source, CPtr dest)
		{
			new(PlaceHolder(), dest) Type(*static_cast<const Type*>(source));
		}

		static void DestructorImpl(const TypeHandler* typeHandler, CPtr instance)
		{
			if constexpr (Traits::IsDestructible<Type>)
			{
				static_cast<Type*>(instance)->~Type();
			}
		};
	};

	template<auto mfp, typename Owner, typename Field>
	class NativeFieldHandler
	{
	public:
		NativeFieldHandler(FieldHandler& fieldHandler) : m_FieldHandler(fieldHandler)
		{
			fieldHandler.SetFnGetFieldInfo(&GetFieldImpl);
			fieldHandler.SetFnGetFieldPointer(&FnGetFieldPointerImpl);
			fieldHandler.SetFnCopyValueTo(&CopyValueToImpl);
			fieldHandler.SetFnSetValue(&SetValue);
		}

	private:
		FieldHandler& m_FieldHandler;

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


	//function

	template<auto fp, typename Return, typename ...Args>
	class NativeFunctionHandler
	{
	public:
		NativeFunctionHandler(FunctionHandler& functionHandler) : m_FunctionHandler(functionHandler)
		{
			m_FunctionHandler.SetFnCall(&CallImpl);
			m_FunctionHandler.SetFunctionPointer(reinterpret_cast<CPtr>(&FunctionImpl));
		}

		template<typename TypeAttr, typename ...AttrArgs>
		inline auto Attribute(AttrArgs&& ... args)
		{
			NativeAttributeHandler<decltype(*this), TypeAttr>::Value = TypeAttr{Traits::Forward<AttrArgs>(args)...};
			NativeAttributeHandler<decltype(*this), TypeAttr>(m_FunctionHandler.NewAttribute(GetTypeID<TypeAttr>()));
			return *this;
		}

	private:
		FunctionHandler& m_FunctionHandler;

		static void CallImpl(const FunctionHandler* handler, CPtr instance, CPtr ret, CPtr* params)
		{
			u32 i{sizeof...(Args)};
			if constexpr (Traits::IsSame<Return, void>)
			{
				fp(*static_cast<Traits::RemoveReference<Args>*>(params[--i])...);
			}
			else
			{
				*static_cast<Traits::RemoveReference<Return>*>(ret) = fp(*static_cast<Traits::RemoveReference<Args>*>(params[--i])...);
			}
		}

		static Return FunctionImpl(const FunctionHandler* handler, CPtr instance, Args... args)
		{
			return fp(args...);
		}
	};


	template<auto mfp, typename Return, typename Owner, typename ...Args>
	class NativeMemberFunctionHandler
	{
	public:
		NativeMemberFunctionHandler(FunctionHandler& functionHandler) : m_FunctionHandler(functionHandler)
		{
			m_FunctionHandler.SetFnCall(&CallImpl);
			m_FunctionHandler.SetFunctionPointer(reinterpret_cast<CPtr>(&FunctionImpl));
		}

		template<typename TypeAttr, typename ...AttrArgs>
		inline auto Attribute(AttrArgs&& ... args)
		{
			NativeAttributeHandler<decltype(*this), TypeAttr>::Value = TypeAttr{Traits::Forward<AttrArgs>(args)...};
			NativeAttributeHandler<decltype(*this), TypeAttr>(m_FunctionHandler.NewAttribute(GetTypeID<TypeAttr>()));
			return *this;
		}

	private:
		FunctionHandler& m_FunctionHandler;

		static void CallImpl(const FunctionHandler* handler, CPtr instance, CPtr ret, CPtr* params)
		{
			u32 i{sizeof...(Args)};
			if constexpr (Traits::IsSame<Return,void>)
			{
				(static_cast<Owner*>(instance)->*mfp)(*static_cast<Traits::RemoveReference<Args>*>(params[--i])...);
			}
			else
			{
				*static_cast<Traits::RemoveReference<Return>*>(ret) = (static_cast<Owner*>(instance)->*mfp)(*static_cast<Traits::RemoveReference<Args>*>(params[--i])...);
			}
		}

		static Return FunctionImpl(const FunctionHandler* handler, CPtr instance, Args... args)
		{
			return (static_cast<Owner*>(instance)->*mfp)(args...);
		}
	};


	template<auto func, typename Function>
	struct MemberFunctionTemplateDecomposer
	{
	};

	template<auto mfp, typename Return, typename Owner>
	struct MemberFunctionTemplateDecomposer<mfp, Return(Owner::*)()>
	{
		using FuncType = MemberFunctionTemplateDecomposer<mfp, Return(Owner::*)()>;

		static decltype(auto) CreateHandler(FunctionHandler& functionHandler)
		{
			return NativeMemberFunctionHandler<mfp, Return, Owner>(functionHandler);
		}

		static FunctionHandlerCreation MakeCreation(const StringView& name)
		{
			return FunctionHandlerCreation{
				.Name = name,
				.FunctionId = GetTypeID<FuncType>(),
				.Return = MakeFieldInfo<Owner, Return>()
			};
		}
	};

	template<auto mfp, typename Return, typename Owner, typename ...Args>
	struct MemberFunctionTemplateDecomposer<mfp, Return(Owner::*)(Args...)>
	{
		using FuncType = MemberFunctionTemplateDecomposer<mfp, Return(Owner::*)()>;
		static constexpr FieldInfo Params[] = {MakeFieldInfo<Owner, Args>()...};

		static decltype(auto) CreateHandler(FunctionHandler& functionHandler)
		{
			return NativeMemberFunctionHandler<mfp, Return, Owner, Args...>(functionHandler);
		}

		static FunctionHandlerCreation MakeCreation(const StringView& name)
		{
			return FunctionHandlerCreation{
				.Name = name,
				.FunctionId = GetTypeID<FuncType>(),
			 	.Params = {Params, sizeof...(Args)},
				.Return = MakeFieldInfo<Owner, Return>()
			};
		}
	};

	template<auto fp, typename Return>
	struct MemberFunctionTemplateDecomposer<fp, Return(*)()>
	{
		using FuncType = MemberFunctionTemplateDecomposer<fp, Return(*)()>;

		static decltype(auto) CreateHandler(FunctionHandler& functionHandler)
		{
			return NativeFunctionHandler<fp, Return>(functionHandler);
		}

		static FunctionHandlerCreation MakeCreation(const StringView& name)
		{
			return FunctionHandlerCreation{
				.Name = name,
				.FunctionId = GetTypeID<FuncType>(),
				.Return = MakeFieldInfo<void, Return>()
			};
		}
	};

	template<auto fp, typename Return, typename ...Args>
	struct MemberFunctionTemplateDecomposer<fp, Return(*)(Args...)>
	{
		using FuncType = MemberFunctionTemplateDecomposer<fp, Return(*)(Args...)>;
		static constexpr FieldInfo Params[] = {MakeFieldInfo<void, Args>()...};

		static decltype(auto) CreateHandler(FunctionHandler& functionHandler)
		{
			return NativeFunctionHandler<fp, Return, Args...>(functionHandler);
		}

		static FunctionHandlerCreation MakeCreation(const StringView& name)
		{
			return FunctionHandlerCreation{
				.Name = name,
				.FunctionId = GetTypeID<FuncType>(),
				.Params = {Params, sizeof...(Args)},
				.Return = MakeFieldInfo<void, Return>()
			};
		}
	};
	//type


	template<typename Type>
	class NativeTypeHandler
	{
	public:
		explicit NativeTypeHandler(TypeHandler& typeHandler) : m_TypeHandler(typeHandler)
		{
			if constexpr (Traits::IsDirectConstructible<Type>)
			{
				this->Constructor();
			}
			typeHandler.SetFnDestroy(&NativeTypeHandlerFuncs<Type>::DestroyImpl);
			typeHandler.SetFnCopy(&NativeTypeHandlerFuncs<Type>::CopyImpl);
			typeHandler.SetFnDestructor(&NativeTypeHandlerFuncs<Type>::DestructorImpl);
		}

		inline auto Constructor()
		{
			return NativeConstructorHandler<Type>(m_TypeHandler.NewConstructor(nullptr, 0));
		}

		template<typename ...Args>
		inline auto Constructor()
		{
			TypeID ids[] = {GetTypeID<Args>()...,};
			return NativeConstructorHandler<Type, Args...>(m_TypeHandler.NewConstructor(ids, sizeof...(Args)));
		}

		template<auto mfp>
		inline auto Field(const StringView& name)
		{
			using FieldDecomp = FieldTemplateDecomposer<mfp, decltype(mfp)>;
			return FieldDecomp::CreateHandler(m_TypeHandler.NewField(name));
		}

		template<auto mfp>
		inline auto Function(const StringView& name)
		{
			using FuncType = Traits::RemoveConstFunc<decltype(mfp)>;
			using DecompType = MemberFunctionTemplateDecomposer<mfp, FuncType>;
			return DecompType::CreateHandler(m_TypeHandler.NewFunction(DecompType::MakeCreation(name)));
		}

		template<typename TypeAttr, typename ...Args>
		inline auto Attribute(Args&& ... args)
		{
			NativeAttributeHandler<Type, TypeAttr>::Value = TypeAttr{Traits::Forward<Args>(args)...};
			NativeAttributeHandler<Type, TypeAttr>(m_TypeHandler.NewAttribute(GetTypeID<TypeAttr>()));
			return *this;
		}

	private:
		TypeHandler& m_TypeHandler;
	};


	class RuntimeFieldHandler
	{
	public:
		explicit RuntimeFieldHandler(FieldHandler& fieldHandler) : m_FieldHandler(fieldHandler)
		{

		}
	private:
		FieldHandler& m_FieldHandler;
	};

	class RuntimeTypeHandler
	{
	public:
		explicit RuntimeTypeHandler(TypeHandler& typeHandler) : m_TypeHandler(typeHandler)
		{

		}

		inline auto Field(const StringView& name, TypeID typeId)
		{
			return RuntimeFieldHandler(m_TypeHandler.NewField(name));
		}

	private:
		TypeHandler& m_TypeHandler;
	};

	namespace Reflection
	{

		SK_API void Init();
		SK_API void Shutdown();

		SK_API TypeHandler& NewType(const StringView& name, const TypeInfo& typeInfo);
		SK_API TypeHandler* FindTypeByName(const StringView& name);
		SK_API TypeHandler* FindTypeById(TypeID typeId);

		template<typename T>
		SK_API TypeHandler* FindType()
		{
			return FindTypeById(GetTypeID<T>());
		}

		template<typename T>
		SK_API inline decltype(auto) Type()
		{
			return NativeTypeHandler<T>(NewType(GetTypeName<T>(), GetTypeInfo<T>()));
		}

		template<typename T>
		SK_API inline decltype(auto) Type(const StringView& name)
		{
			return NativeTypeHandler<T>(NewType(name, GetTypeInfo<T>()));
		}

		SK_API inline decltype(auto) Type(const StringView& name, TypeID typeId)
		{
			return RuntimeTypeHandler(NewType(name, TypeInfo{
				.TypeId = typeId
			}));
		}
	}
}