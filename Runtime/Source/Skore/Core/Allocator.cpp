// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.


#include <cstdlib>
#include "Allocator.hpp"

namespace Skore
{
	CPtr DefaultAllocatorMemAlloc(CPtr alloc, usize bytes)
	{
		return std::malloc(bytes);
	}

	void DefaultAllocatorMemFree(CPtr alloc, CPtr ptr, usize bytes)
	{
		std::free(ptr);
	}

	Allocator allocator{
		.Alloc = nullptr,
		.MemAlloc = DefaultAllocatorMemAlloc,
		.MemFree = DefaultAllocatorMemFree
	};

	Allocator* GetDefaultAllocator()
	{
		return &allocator;
	}

	void SetDefaultAllocator(const Allocator& pAllocator)
	{
		allocator = pAllocator;
	}
}