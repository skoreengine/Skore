//  Copyright 2023 Paulo Marangoni.
//  Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Skore/Platform/Platform.hpp"

namespace Skore::Random
{
	inline u64 Xorshift64star()
	{
		static u64 x = static_cast<u64>(Platform::GetTime());
		x ^= x >> 12;
		x ^= x << 25;
		x ^= x >> 27;
		return x * 0x2545F4914F6CDD1DULL;
	}

	inline i64 NextInt(i64 max)
	{
		static i64 x = static_cast<i64>(Platform::GetTime());
		x ^= x >> 12;
		x ^= x << 25;
		x ^= x >> 27;
		x *= 0x2545F4914F6CDD1DULL;
		return (x % max);
	}

	inline u64 NextUInt(u64 max)
	{
		static u64 x = static_cast<u64>(Platform::GetTime());
		x ^= x >> 12;
		x ^= x << 25;
		x ^= x >> 27;
		x *= 0x2545F4914F6CDD1DULL;
		return (x % max);
	}
}