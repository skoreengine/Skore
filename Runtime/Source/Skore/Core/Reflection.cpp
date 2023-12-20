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
		HashMap<String, Array<SharedPtr<TypeHandler>>> typesByName{};
		HashMap<TypeID, Array<SharedPtr<TypeHandler>>> typesByID{};

		Logger& logger = Logger::GetLogger("Reflection");

		virtual ~ReflectionContext()
		{
			SK_ASSERT(typesByName.Empty(), "Was the Reflection::Shutdown called?");
			SK_ASSERT(typesByID.Empty(), "Was the Reflection::Shutdown called?");
		}
	};

	ReflectionContext reflection{};

	TypeHandler::TypeHandler(const StringView& name, u32 version) : m_name(name), m_version(version)
	{
	}

	void TypeHandler::SetDestroyFn(DestroyFn destroyFn)
	{
		m_destroyFn = destroyFn;
	}

	ConstructorHandler& TypeHandler::NewConstructor(TypeID* ids, usize size)
	{
		u64 constructorId = size > 0 ? MurmurHash64(ids, size * sizeof(TypeID), HashSeed64) : 0;
		auto it = m_constructors.Find(constructorId);

		if (it == m_constructors.end())
		{
			it = m_constructors.Emplace(constructorId, MakeShared<ConstructorHandler>()).first;
			m_constructorArray.EmplaceBack(it->second.Get());
		}
		return *it->second;
	}

	ConstructorHandler* TypeHandler::FindConstructor(TypeID* ids, usize size) const
	{
		u64 constructorId = size > 0 ? MurmurHash64(ids, size * sizeof(TypeID), HashSeed64) : 0;
		if (auto it = m_constructors.Find(constructorId))
		{
			return it->second.Get();
		}
		return nullptr;
	}

	FieldHandler& TypeHandler::NewField(const StringView& fieldName)
	{
		auto it = m_fields.Find(fieldName);
		if (it == m_fields.end())
		{
			it = m_fields.Emplace(fieldName, MakeShared<FieldHandler>(fieldName)).first;
			m_fieldArray.EmplaceBack(it->second.Get());
		}
		return *it->second;
	}

	FieldHandler* TypeHandler::FindField(const StringView& fieldName)
	{
		if (auto it = m_fields.Find(fieldName))
		{
			return it->second.Get();
		}
		return nullptr;
	}

	void TypeHandler::Destroy(CPtr instance, Allocator* allocator) const
	{
		if (m_destroyFn)
		{
			m_destroyFn(this, allocator, instance);
		}
	}



	TypeHandler& Reflection::NewType(const StringView& name, const TypeInfo& typeInfo)
	{
		auto itByName = reflection.typesByName.Find(name);
		if (!itByName)
		{
			itByName = reflection.typesByName.Emplace(name, Array<SharedPtr<TypeHandler>>{}).first;
		}

		auto itById = reflection.typesByID.Find(typeInfo.typeId);
		if (!itById)
		{
			itById = reflection.typesByID.Emplace(typeInfo.typeId, Array<SharedPtr<TypeHandler>>{}).first;
		}

		usize version = itByName->second.Size() + 1;
		SharedPtr<TypeHandler> typeHandler = MakeShared<TypeHandler>(name, version);

		itByName->second.EmplaceBack(typeHandler);
		itById->second.EmplaceBack(typeHandler);

		reflection.logger.Debug("Type {} Registered ", name, version);

		return *typeHandler;
	}

	void Reflection::Shutdown()
	{
		reflection.typesByID.Clear();
		reflection.typesByName.Clear();
	}

	TypeHandler* Reflection::FindTypeByName(const StringView& name)
	{
		if (auto it = reflection.typesByName.Find(name))
		{
			return it->second.Back().Get();
		}
		return nullptr;
	}

	void ConstructorHandler::SetPlacementNewFn(ConstructorHandler::PlacementNewFn placementNewFn)
	{
		m_placementNewFn = placementNewFn;
	}

	void ConstructorHandler::SetNewInstanceFn(ConstructorHandler::NewInstanceFn newInstanceFn)
	{
		m_newInstanceFn = newInstanceFn;
	}

	CPtr ConstructorHandler::NewInstance(Allocator* allocator, CPtr* params)
	{
		if (m_newInstanceFn)
		{
			return m_newInstanceFn(this, allocator, params);
		}
		return nullptr;
	}

	FieldHandler::FieldHandler(const String& name) : m_name(name)
	{}

	void FieldHandler::SetFnGetFieldInfo(FnGetFieldInfo fieldInfoFn)
	{
		m_fnGetFieldInfo = fieldInfoFn;
	}

	FieldInfo FieldHandler::GetFieldInfo() const
	{
		if (m_fnGetFieldInfo)
		{
			return m_fnGetFieldInfo(this);
		}
		return {};
	}

	CPtr FieldHandler::GetFieldPointer(CPtr instance) const
	{
		if (m_fnGetFieldPointer)
		{
			return m_fnGetFieldPointer(this, instance);
		}
		return nullptr;
	}

	void FieldHandler::SetFnGetFieldPointer(FieldHandler::FnGetFieldPointer fnGetFieldPointer)
	{
		m_fnGetFieldPointer = fnGetFieldPointer;
	}
}
