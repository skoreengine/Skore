// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Reflection.hpp"
#include "HashMap.hpp"
#include "String.hpp"
#include "SharedPtr.hpp"
#include "Array.hpp"

namespace Skore
{
	struct ReflectionContext
	{
		HashMap<String, Array<SharedPtr<ReflType>>> typesByName{};
		HashMap<TypeID, Array<SharedPtr<ReflType>>> typesByID{};

		virtual ~ReflectionContext()
		{
			SK_ASSERT(typesByName.Empty(), "Was the Reflection::Shutdown called?");
			SK_ASSERT(typesByID.Empty(), "Was the Reflection::Shutdown called?");
		}
	};

	ReflectionContext reflection{};

	ReflType::ReflType(const StringView& name, u32 version) : m_name(name), m_version(version)
	{

	}

	ReflType& Reflection::NewType(const StringView& name, const TypeInfo& typeInfo)
	{
		auto itByName = reflection.typesByName.Find(name);
		if (!itByName)
		{
			itByName = reflection.typesByName.Emplace(name, Array<SharedPtr<ReflType>>{}).first;
		}

		auto itById = reflection.typesByID.Find(typeInfo.typeId);
		if (!itById)
		{
			itById = reflection.typesByID.Emplace(typeInfo.typeId, Array<SharedPtr<ReflType>>{}).first;
		}

		usize version = itByName->second.Size() + 1;

		SharedPtr<ReflType> reflType = MakeShared<ReflType>(name, version);

		itByName->second.EmplaceBack(reflType);
		itById->second.EmplaceBack(reflType);


		return *reflType;
	}

	void Reflection::Shutdown()
	{
		reflection.typesByID.Clear();
		reflection.typesByName.Clear();
	}

}
