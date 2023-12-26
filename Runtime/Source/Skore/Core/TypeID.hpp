// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Hash.hpp"
#include "Traits.hpp"
#include "StringView.hpp"

namespace Skore
{

	namespace Internal
	{
		template<typename Type>
		constexpr auto StrippedTypeName()
		{
			StringView prettyFunction = SK_PRETTY_FUNCTION;
			usize first = prettyFunction.FindFirstNotOf(' ', prettyFunction.FindFirstOf(SK_PRETTY_FUNCTION_PREFIX) + 1);
			StringView value = prettyFunction.Substr(first, prettyFunction.FindLastOf(SK_PRETTY_FUNCTION_SUFFIX) - first);
			return value;
		}

		template<typename Type>
		constexpr auto GetTypeName()
		{
			StringView typeName = StrippedTypeName<Type>();

			usize space = typeName.FindFirstOf(' ');
			if (space != StringView::s_npos)
			{
				return typeName.Substr(space + 1);
			}
			return typeName;
		}

		template<typename Type>
		constexpr static TypeID GetTypeID()
		{
			constexpr TypeID typeId = Hash<StringView>::Value(StrippedTypeName<Type>());
			return typeId;
		}
	}

	template<typename Type>
	constexpr static TypeID GetTypeID()
	{
		return Internal::GetTypeID<Traits::RemoveAll<Type>>();
	}

	template<typename Type>
	constexpr static StringView GetTypeName()
	{
		return Internal::GetTypeName<Traits::RemoveAll<Type>>();
	}

	template<typename Type>
	constexpr static usize GetTypeSize()
	{
		if constexpr (Traits::IsComplete<Traits::RemoveAll<Type>>)
		{
			return sizeof(Traits::RemoveAll<Type>);
		}
		return 0;
	}

}