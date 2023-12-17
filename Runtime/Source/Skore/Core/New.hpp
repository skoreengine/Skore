// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"

namespace Skore
{
	struct PlaceHolder
	{
	};
}

inline void* operator new(usize, Skore::PlaceHolder, CPtr ptr)
{
	return ptr;
}

inline void operator delete(void*, Skore::PlaceHolder, CPtr) noexcept
{
}