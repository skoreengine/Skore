// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"

namespace Skore
{
#define SK_UINT64_C(c) c ## ULL

	template<typename T, typename Enable = void>
	struct Hash
	{
		constexpr static bool HasHash = false;
	};

	//based on https://xorshift.di.unimi.it/splitmix64.c
#define IMPL_HASH_64(_TYPE)                         \
    template<>                                      \
    struct Hash<_TYPE>                              \
    {                                               \
        constexpr static bool HasHash = true;       \
        constexpr static usize Value(_TYPE x)       \
        {                                           \
            x = (x ^ (x >> 30)) * SK_UINT64_C(0xbf58476d1ce4e5b9); \
            x = (x ^ (x >> 27)) * SK_UINT64_C(0x94d049bb133111eb); \
            x = x ^ (x >> 31);                                  \
            return x;                                           \
        }                                                       \
    }

#define IMPL_HASH_32(_TYPE)\
    template<>  \
    struct Hash<_TYPE>     \
    {                                \
        constexpr static bool HasHash = true;           \
        template<typename Type>                         \
        constexpr static usize Value(Type vl)           \
        {                                               \
            auto x = static_cast<usize>(vl);            \
            x = ((x >> 16) ^ x) * 0x119de1f3;           \
            x = ((x >> 16) ^ x) * 0x119de1f3;           \
            x = (x >> 16) ^ x;                          \
            return x;                                   \
        }                                               \
    }

	IMPL_HASH_64(u64);
	IMPL_HASH_64(i64);

	IMPL_HASH_32(i32);
	IMPL_HASH_32(u32);
	IMPL_HASH_32(ul32);
	IMPL_HASH_32(bool);


	template<>
	struct Hash<char>
	{
		constexpr static bool HasHash = true;
		constexpr static usize Value(const char* ch)
		{
			usize hash = 0;
			for (const char* c = ch; *c != '\0'; ++c)
			{
				hash = *ch + (hash << 6) + (hash << 16) - hash;
			}
			return hash;
		}
	};

}