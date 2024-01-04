// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "EntryPoint.hpp"

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

#include <atomic>
#include <unordered_map>

#include "Skore/Core/Allocator.hpp"

using namespace Skore;


std::atomic_uint64_t totalAlloc;
std::atomic_int64_t mapAlloc;

struct MemAllocation
{
	usize bytes;
};

struct Allocations
{
	std::unordered_map<usize, MemAllocation> alloc;
};

std::unordered_map<usize, MemAllocation>& GetAllocations()
{
	static Allocations* allocations{};
	if (allocations == nullptr)
	{
		allocations = new Allocations;
	}
	return allocations->alloc;
}


CPtr TestAllocatorMemAlloc(CPtr alloc, usize bytes)
{
	totalAlloc += bytes;
	mapAlloc += bytes;
	CPtr memory = std::malloc(bytes);
	usize address = reinterpret_cast<usize>(memory);
	GetAllocations().insert(std::make_pair(address, MemAllocation{bytes}));
	return memory;
}

void TestAllocatorMemFree(CPtr alloc, CPtr ptr, usize bytes)
{
	if (!ptr) return;

	usize address = reinterpret_cast<usize>(ptr);
	mapAlloc -= bytes;
	std::free(ptr);

	GetAllocations().erase(address);
}


int main(int argc, char** argv)
{
	SetDefaultAllocator(Allocator{
		.Alloc = nullptr,
		.MemAlloc = TestAllocatorMemAlloc,
		.MemFree = TestAllocatorMemFree
	});

	doctest::Context context;
	context.applyCommandLine(argc, argv);
	context.setOption("no-breaks", true);

	i32 res = context.run();

	std::cout << "[Skore] Total Alloc : " << totalAlloc << std::endl;
	std::cout << "[Skore] Alloc End : " << mapAlloc << std::endl;
	std::cout << "[Skore] Pending Dealloc: " << GetAllocations().size() << std::endl;

	return res + GetAllocations().size();
}

usize GetAllocationNum()
{
	return GetAllocations().size();
}
