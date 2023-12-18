// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Algorithm.hpp"

namespace Skore
{

	template<typename Type, typename Enable = void>
	struct StringConverter
	{
		constexpr static bool  HasConverter = false;
		constexpr static usize BufferCount  = 0;
	};

#define SK_NUMERIC_CONVERTER(_TYPE, _SIZE, _CHAR) \
    template<>                                          \
    struct StringConverter<_TYPE> {                     \
        constexpr static bool  HasConverter = true;     \
        constexpr static usize BufferCount = _SIZE;     \
                                                        \
        static usize Size(const _TYPE& value)           \
        {                                               \
            return 0;  \
        }                                               \
                                                                            \
        static usize ToString(char* buffer, usize pos, const _TYPE &value)  \
        {                                                                   \
            return 0;                                                       \
        }                                                                   \
                                                                            \
        static void FromString(const char* str, usize size, _TYPE& value)   \
        {                                                                   \
                                                                            \
        }                                                                   \
    };

	SK_NUMERIC_CONVERTER(u8, 5, "%u")
	SK_NUMERIC_CONVERTER(u16, 10, "%u")
	SK_NUMERIC_CONVERTER(u32, 10, "%u")
	SK_NUMERIC_CONVERTER(u64, 21, "%llu")

	SK_NUMERIC_CONVERTER(i8, 10, "%d")
	SK_NUMERIC_CONVERTER(i16, 10, "%d")
	SK_NUMERIC_CONVERTER(i32, 10, "%d")
	SK_NUMERIC_CONVERTER(ul32, 21, "%lu")
	SK_NUMERIC_CONVERTER(i64, 10, "%lld")


	template<>
	struct StringConverter<char>
	{
		constexpr static bool  HasConverter = true;
		constexpr static usize BufferCount  = 0;

		static usize Size(const char* chars)
		{
			return Strlen(chars);
		}

		static usize ToString(char* buffer, usize pos, const char* chars)
		{
			auto len = Strlen(chars);
			StrCopy(buffer, pos, chars, len);
			return len;
		}

		static void FromString(const char* str, usize size, char* ch)
		{

		}
	};

}