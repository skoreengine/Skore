// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Algorithm.hpp"

namespace Skore
{

	namespace Converters
	{
		SK_API usize U32ToString(char* buffer, usize bufferSize, u32 value);
		SK_API usize U64ToString(char* buffer, usize bufferSize, u64 value);
		SK_API usize I32ToString(char* buffer, usize bufferSize, i32 value);
		SK_API usize I64ToString(char* buffer, usize bufferSize, i64 value);
		SK_API usize F32ToString(char* buffer, usize bufferSize, f32 value);
		SK_API usize F64ToString(char* buffer, usize bufferSize, f64 value);

		SK_API u32 U32FromString(const char* str);
		SK_API u64 U64FromString(const char* str);
		SK_API i32 I32FromString(const char* str);
		SK_API i64 I64FromString(const char* str);
		SK_API f32 F32FromString(const char* str);
		SK_API f64 F64FromString(const char* str);
	}

	template<typename Type, typename Enable = void>
	struct StringConverter
	{
		constexpr static bool  HasConverter = false;
		constexpr static usize BufferCount  = 0;
	};

	template<>
	struct StringConverter<u32>
	{
		constexpr static bool  HasConverter = true;
		constexpr static usize BufferCount  = 10;

		static usize Size(const u32& value)
		{
			return Converters::U32ToString(nullptr, 0, value);
		}

		static usize ToString(char* buffer, usize pos, const u32& value)
		{
			return Converters::U32ToString(buffer + pos, BufferCount, value);
		}

		static void FromString(const char* str, usize size, u32& value)
		{
			value = Converters::U32FromString(str);
		}
	};

	template<>
	struct StringConverter<u64>
	{
		constexpr static bool  HasConverter = true;
		constexpr static usize BufferCount  = 21;

		static usize Size(const u64& value)
		{
			return Converters::U64ToString(nullptr, 0, value);
		}

		static usize ToString(char* buffer, usize pos, const u64& value)
		{
			return Converters::U64ToString(buffer + pos, BufferCount, value);
		}

		static void FromString(const char* str, usize size, u64& value)
		{
			value = Converters::U64FromString(str);
		}
	};

    template<>
    struct StringConverter<ul32>
    {
        constexpr static bool  HasConverter = true;
        constexpr static usize BufferCount  = 21;

        static usize Size(const ul32& value)
        {
            return Converters::U64ToString(nullptr, 0, value);
        }

        static usize ToString(char* buffer, usize pos, const ul32& value)
        {
            return Converters::U32ToString(buffer + pos, BufferCount, value);
        }

        static void FromString(const char* str, usize size, ul32& value)
        {
            value = Converters::U32FromString(str);
        }
    };

	template<>
	struct StringConverter<i32>
	{
		constexpr static bool  HasConverter = true;
		constexpr static usize BufferCount  = 10;

		static usize Size(const i32& value)
		{
			return Converters::I32ToString(nullptr, 0, value);
		}

		static usize ToString(char* buffer, usize pos, const i32& value)
		{
			return Converters::I32ToString(buffer + pos, BufferCount, value);
		}

		static void FromString(const char* str, usize size, i32& value)
		{
			value = Converters::I32FromString(str);
		}
	};

	template<>
	struct StringConverter<i64>
	{
		constexpr static bool  HasConverter = true;
		constexpr static usize BufferCount  = 25;

		static usize Size(const i64& value)
		{
			return Converters::I64ToString(nullptr, 0, value);
		}

		static usize ToString(char* buffer, usize pos, const i64& value)
		{
			return Converters::I64ToString(buffer + pos, BufferCount, value);
		}

		static void FromString(const char* str, usize size, i64& value)
		{
			value = Converters::I64FromString(str);
		}
	};

	template<>
	struct StringConverter<f32>
	{
		constexpr static bool  HasConverter = true;
		constexpr static usize BufferCount  = 25;

		static usize Size(const f32& value)
		{
			return Converters::F32ToString(nullptr, 0, value);
		}

		static usize ToString(char* buffer, usize pos, const f32& value)
		{
			return Converters::F32ToString(buffer + pos, BufferCount, value);
		}

		static void FromString(const char* str, usize size, f32& value)
		{
			value = Converters::F32FromString(str);
		}
	};

	template<>
	struct StringConverter<f64>
	{
		constexpr static bool  HasConverter = true;
		constexpr static usize BufferCount  = 25;

		static usize Size(const f32& value)
		{
			return Converters::F64ToString(nullptr, 0, value);
		}

		static usize ToString(char* buffer, usize pos, const f64& value)
		{
			return Converters::F64ToString(buffer + pos, BufferCount, value);
		}

		static void FromString(const char* str, usize size, f64& value)
		{
			value = Converters::F64FromString(str);
		}
	};

	template<usize T>
	struct StringConverter<char[T]>
	{

		constexpr static bool  HasConverter = true;
		constexpr static usize BufferCount  = T - 1;

		static usize Size(const char chars[T])
		{
			return BufferCount;
		}

		static usize ToString(char* buffer, usize pos, const char chars[T])
		{
			StrCopy(buffer, pos, chars, BufferCount);
			return BufferCount;
		}

		static void FromString(const char* str, usize size, char ch[T])
		{

		}
	};

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