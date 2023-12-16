// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"

namespace Skore
{
	struct SK_API Allocator
	{
		CPtr alloc;
		CPtr (*MemAlloc)(CPtr alloc, usize bytes);
		void (*MemFree)(CPtr alloc, CPtr ptr, usize bytes);
	};

	SK_API Allocator* GetDefaultAllocator();

}