// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
namespace Skore
{

	template<typename Type>
	inline void StrCopy(Type* dest, const Type* origin, usize size)
	{
		for (int i = 0; i < size; ++i)
		{
			dest[i] = origin[i];
		}
	}

	template<typename Type>
	inline void StrCopy(Type* dest, usize destPos, const Type* origin, usize size)
	{
		for (int i = 0; i < size; ++i)
		{
			dest[destPos + i] = origin[i];
		}
	}

	constexpr usize Strlen(const char* s)
	{
		for (usize len = 0;; ++len)
		{
			if (0 == s[len])
			{
				return len;
			}
		}
		return 0;
	}

}

