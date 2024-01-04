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

	struct ResourceFieldCreation
	{
		u32         Index{U32_MAX};
		StringView  Name{};
		TypeID      Type{};
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