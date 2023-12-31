// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Skore/Core/Hash.hpp"
#include "Skore/Core/StringView.hpp"
#include "Skore/Core/String.hpp"
#include "Skore/Core/Span.hpp"

namespace Skore
{
	struct RID
	{
		union
		{
			struct
			{
				u32 Offset;
				u32 Page;
			};
			u64 Id{};
		};

		inline explicit operator bool() const noexcept
		{
			return this->Offset > 0 || this->Page > 0;
		}

		inline bool operator==(const RID& rid) const
		{
			return this->Id == rid.Id;
		}

		inline bool operator!=(const RID& rid) const
		{
			return !((*this) == rid);
		}
	};

	template<>
	struct Hash<RID>
	{
		constexpr static bool hasHash = true;
		constexpr static usize Value(const RID& rid)
		{
			return HashValue(rid.Id);
		}
	};

	enum ResourceFieldType_
	{
		ResourceFieldType_Undefined    = 0,
		ResourceFieldType_Value        = 1,
		ResourceFieldType_SubObject    = 2,
		ResourceFieldType_SubObjectSet = 3,
		ResourceFieldType_Reference    = 4,
		ResourceFieldType_ReferenceSet = 5,
		ResourceFieldType_Buffer       = 6
	};

	typedef u32 ResourceFieldType;

	struct ResourceFieldCreation
	{
		u32               Index{U32_MAX};
		StringView        Name{};
		ResourceFieldType Type{ResourceFieldType_Value};
		TypeID            FieldTypeId{};
	};

	struct ResourceTypeCreation
	{
		StringView                  Name{};
		TypeID                      TypeId{};
		Span<ResourceFieldCreation> Fields{};
	};

	struct ResourceStorage;
	struct ResourceData;
}