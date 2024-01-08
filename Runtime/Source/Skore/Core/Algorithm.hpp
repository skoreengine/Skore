// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"

namespace Skore
{

	template<typename Type>
	class BasicStringView;

	template<typename Element, typename Func>
	inline void Split(const BasicStringView<Element>& string, const BasicStringView<Element>& delimiter, Func&& func)
	{
		usize start = 0;
		for (int i = 0; i < string.Size(); ++i)
		{
			BasicStringView<Element> comp{string.Data() + i, delimiter.Size()};
			if (comp == delimiter)
			{
				BasicStringView<Element> split{string.Data() + start, i - start};
				if (!split.Empty())
				{
					func(split);
				}
				start = i + delimiter.Size();
			}
		}
		BasicStringView<Element> split{string.Data() + start, string.Size() - start};
		if (!split.Empty())
		{
			func(split);
		}
	}

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

	template<typename T>
	inline constexpr void Swap(T& a, T& b)
	{
		T temp = (T&&)a;
		a = (T&&)b;
		b = (T&&)temp;
	}

	template<typename T, typename F>
	inline T* Partition(T* begin, T* end, const F& comp)
	{
		T* pivot = end - 1;
		T* i = begin - 1;
		for (T* j = begin; j < end - 1; ++j)
		{
			if (comp(*j, *pivot))
			{
				++i;
				Swap(*i, *j);
			}
		}
		Swap(*(i + 1), *pivot);
		return i + 1;
	}

	template<typename T, typename F>
	inline void Sort(T* begin, T* end, const F& comp)
	{
		if (begin < end)
		{
			T* pivot = Partition(begin, end, comp);
			Sort(begin, pivot, comp);
			Sort(pivot + 1, end, comp);
		}
	}

	template<typename Element>
	inline u64 HexTo64(const BasicStringView<Element>& str) {
		u64 res = 0;
		for (const char c: str) {
			char v = (c & 0xF) + (c >> 6) | ((c >> 3) & 0x8);
			res = (res << 4) | (u64) v;
		}
		return res;
	}

	inline void MemSet(CPtr desc, char value, usize size)
	{
		for (usize i = 0; i < size; ++i)
		{
			*((char*) desc + i) = value;
		}
	}
}

