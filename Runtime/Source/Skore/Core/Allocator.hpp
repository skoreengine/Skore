// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Traits.hpp"
#include "New.hpp"

namespace Skore
{
	struct SK_API Allocator
	{
		CPtr Alloc;
		CPtr (*MemAlloc)(CPtr alloc, usize bytes);
		void (*MemFree)(CPtr alloc, CPtr ptr, usize bytes);
	};

	typedef Allocator* AllocatorPtr;

	SK_API Allocator* GetDefaultAllocator();
	SK_API void SetDefaultAllocator(const Allocator& allocator);

	template<typename Type, typename ...Args>
	static Type* Alloc(Args&& ...args)
	{
		auto ptr = static_cast<Type*>(GetDefaultAllocator()->MemAlloc(GetDefaultAllocator()->Alloc, sizeof(Type)));
		if constexpr (Traits::IsAggregate<Type>)
		{
			new(PlaceHolder(), ptr) Type{Traits::Forward<Args>(args)...};
		}
		else
		{
			new(PlaceHolder(), ptr) Type(Traits::Forward<Args>(args)...);
		}
		return ptr;
	}

	template<typename Type>
	static void Destroy(Type * type)
	{
		type->~Type();
		GetDefaultAllocator()->MemFree(GetDefaultAllocator()->Alloc, type, sizeof(Type));
	}

	template<typename Type, typename ...Args>
	static Type* Alloc(Allocator* allocator, Args&& ...args)
	{
		auto ptr = static_cast<Type*>(allocator->MemAlloc(allocator->Alloc, sizeof(Type)));
		if constexpr (Traits::IsAggregate<Type>)
		{
			new(PlaceHolder(), ptr) Type{Traits::Forward<Args>(args)...};
		}
		else
		{
			new(PlaceHolder(), ptr) Type(Traits::Forward<Args>(args)...);
		}
		return ptr;
	}

	template<typename Type>
	static void Destroy(Allocator* allocator, Type * type)
	{
		type->~Type();
		allocator->MemFree(allocator->Alloc, type, sizeof(Type));
	}

}