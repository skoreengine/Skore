// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Hash.hpp"
#include "Traits.hpp"

namespace Skore
{

	namespace Internal
	{
		template<typename Type>
		constexpr static TypeID GetTypeID()
		{
			constexpr TypeID typeId = Hash<char>::Value(SK_PRETTY_FUNCTION);
			return typeId;
		}
	}

	template<typename Type>
	constexpr static TypeID GetTypeID()
	{
		return Internal::GetTypeID<Traits::RemoveAll<Type>>();
	}

}