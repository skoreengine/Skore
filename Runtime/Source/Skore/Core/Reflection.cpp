// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Reflection.hpp"
#include "HashMap.hpp"
#include "String.hpp"
#include "SharedPtr.hpp"
#include "Array.hpp"
#include "Logger.hpp"

namespace Skore
{
	struct ReflectionContext
	{
		HashMap<String, Array<SharedPtr<TypeHandler>>> TypesByName{};
		HashMap<TypeID, Array<SharedPtr<TypeHandler>>> TypesByID{};

		Logger& Logger = Logger::GetLogger("Reflection");

		virtual ~ReflectionContext()
		{
			SK_ASSERT(TypesByName.Empty(), "Was the Reflection::Shutdown called?");
			SK_ASSERT(TypesByID.Empty(), "Was the Reflection::Shutdown called?");
		}
	};

	ReflectionContext reflection{};

	TypeHandler::TypeHandler(const StringView& name, u32 version) : m_Name(name), m_Version(version)
	{
	}

	void TypeHandler::SetDestroyFn(DestroyFn destroyFn)
	{
		m_DestroyFn = destroyFn;
	}

	ConstructorHandler& TypeHandler::NewConstructor(TypeID* ids, usize size)
	{
		u64 constructorId = size > 0 ? MurmurHash64(ids, size * sizeof(TypeID), HashSeed64) : 0;
		auto it = m_Constructors.Find(constructorId);

		if (it == m_Constructors.end())
		{
			it = m_Constructors.Emplace(constructorId, MakeShared<ConstructorHandler>()).First;
			m_ConstructorArray.EmplaceBack(it->Second.Get());
		}
		return *it->Second;
	}

	ConstructorHandler* TypeHandler::FindConstructor(TypeID* ids, usize size) const
	{
		u64 constructorId = size > 0 ? MurmurHash64(ids, size * sizeof(TypeID), HashSeed64) : 0;
		if (auto it = m_Constructors.Find(constructorId))
		{
			return it->Second.Get();
		}
		return nullptr;
	}

	Span<ConstructorHandler*> TypeHandler::GetConstructors() const
	{
		return m_ConstructorArray;
	}

	FieldHandler& TypeHandler::NewField(const StringView& fieldName)
	{
		auto it = m_Fields.Find(fieldName);
		if (it == m_Fields.end())
		{
			it = m_Fields.Emplace(fieldName, MakeShared<FieldHandler>(fieldName)).First;
			m_FieldArray.EmplaceBack(it->Second.Get());
		}
		return *it->Second;
	}

	FieldHandler* TypeHandler::FindField(const StringView& fieldName) const
	{
		if (auto it = m_Fields.Find(fieldName))
		{
			return it->Second.Get();
		}
		return nullptr;
	}

	Span<FieldHandler*> TypeHandler::GetFields() const
	{
		return m_FieldArray;
	}

	void TypeHandler::Destroy(CPtr instance, Allocator* allocator) const
	{
		if (m_DestroyFn)
		{
			m_DestroyFn(this, allocator, instance);
		}
	}

	TypeHandler& Reflection::NewType(const StringView& name, const TypeInfo& typeInfo)
	{
		auto itByName = reflection.TypesByName.Find(name);
		if (!itByName)
		{
			itByName = reflection.TypesByName.Emplace(name, Array<SharedPtr<TypeHandler>>{}).First;
		}

		auto itById = reflection.TypesByID.Find(typeInfo.TypeId);
		if (!itById)
		{
			itById = reflection.TypesByID.Emplace(typeInfo.TypeId, Array<SharedPtr<TypeHandler>>{}).First;
		}

		usize version = itByName->Second.Size() + 1;
		SharedPtr<TypeHandler> typeHandler = MakeShared<TypeHandler>(name, version);

		itByName->Second.EmplaceBack(typeHandler);
		itById->Second.EmplaceBack(typeHandler);

		reflection.Logger.Debug("Type {} Registered ", name, version);

		return *typeHandler;
	}

	void Reflection::Shutdown()
	{
		reflection.TypesByID.Clear();
		reflection.TypesByName.Clear();
	}

	TypeHandler* Reflection::FindTypeByName(const StringView& name)
	{
		if (auto it = reflection.TypesByName.Find(name))
		{
			return it->Second.Back().Get();
		}
		return nullptr;
	}

	void AttributeHandler::SetFnGetValue(AttributeHandler::FnGetValue fnGetValue)
	{
		m_FnGetValue = fnGetValue;
	}

	ConstCPtr AttributeHandler::GetValue()
	{
		if (m_FnGetValue)
		{
			return m_FnGetValue(this);
		}
		return nullptr;
	}

	AttributeHandler::AttributeHandler(TypeID typeId, usize size) : m_TypeId(typeId), m_Size(size)
	{}

	void ConstructorHandler::SetPlacementNewFn(ConstructorHandler::PlacementNewFn placementNewFn)
	{
		m_PlacementNewFn = placementNewFn;
	}

	void ConstructorHandler::SetNewInstanceFn(ConstructorHandler::NewInstanceFn newInstanceFn)
	{
		m_NewInstanceFn = newInstanceFn;
	}

	CPtr ConstructorHandler::NewInstance(Allocator* allocator, CPtr* params)
	{
		if (m_NewInstanceFn)
		{
			return m_NewInstanceFn(this, allocator, params);
		}
		return nullptr;
	}

	FieldHandler::FieldHandler(const String& name) : m_Name(name)
	{}

	void FieldHandler::SetFnGetFieldInfo(FnGetFieldInfo fieldInfoFn)
	{
		m_FnGetFieldInfo = fieldInfoFn;
	}

	FieldInfo FieldHandler::GetFieldInfo() const
	{
		if (m_FnGetFieldInfo)
		{
			return m_FnGetFieldInfo(this);
		}
		return {};
	}

	CPtr FieldHandler::GetFieldPointer(CPtr instance) const
	{
		if (m_FnGetFieldPointer)
		{
			return m_FnGetFieldPointer(this, instance);
		}
		return nullptr;
	}

	void FieldHandler::SetFnGetFieldPointer(FieldHandler::FnGetFieldPointer fnGetFieldPointer)
	{
		m_FnGetFieldPointer = fnGetFieldPointer;
	}

	void FieldHandler::CopyValueTo(ConstCPtr instance, CPtr value)
	{
		if (m_FnCopyValueTo)
		{
			m_FnCopyValueTo(this, instance, value);
		}
	}

	void FieldHandler::SetValue(CPtr instance, ConstCPtr value)
	{
		if (m_FnSetValue)
		{
			m_FnSetValue(this, instance, value);
		}
	}

	void FieldHandler::SetFnCopyValueTo(FnCopyValueTo fnCopyValueTo)
	{
		m_FnCopyValueTo = fnCopyValueTo;
	}

	void FieldHandler::SetFnSetValue(FnSetValue fnSetValue)
	{
		m_FnSetValue = fnSetValue;
	}
}
