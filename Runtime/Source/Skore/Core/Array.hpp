// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Traits.hpp"
#include "New.hpp"

namespace Skore
{

	template<typename T>
	class Array
	{
	public:
		Array();
//		Array(const Array& other);
//		Array(Array&& other);
//		Array(usize size);
//		Array(usize size, const T& value);
//		Array(const T* first, const T* last);


		void Reserve(usize size);
		void Resize(usize size);
		void Resize(usize size, const T& value);
		void Clear();

		template<typename ...Args>
		T& EmplaceBack(Args&&... args);

		~Array();
	private:
		T* m_first{};
		T* m_last{};
		T* m_capacity{};

		Allocator* m_allocator = GetDefaultAllocator();
	};

	template<typename T>
	Array<T>::Array() : m_first(0), m_last(0), m_capacity(0)
	{
	}

	template<typename T>
	SK_FINLINE void Array<T>::Reserve(usize newCapacity)
	{
		if (m_first + newCapacity <= m_capacity)
		{
			return;
		}

		const usize size = m_last - m_first;

		T* newFirst = (T*) m_allocator->MemAlloc(m_allocator->alloc, sizeof(T) * newCapacity);

		for (T* it = m_first; it != m_last; ++it, ++newFirst)
		{
			new(PlaceHolder(), newFirst) T(Traits::Forward<T>(*it));
			it->~T();
		}

		m_allocator->MemFree(m_allocator->alloc, m_first, (usize) ((char*) m_capacity - (char*) m_first));

		m_first    = newFirst;
		m_last     = newFirst + size;
		m_capacity = newFirst + newCapacity;
	}

	template<typename T>
	SK_FINLINE void Array<T>::Resize(usize size)
	{
		Reserve(size);

		for (T* it = m_last; it < m_first + size; ++it)
		{
			new(PlaceHolder(), it) T();
		}

		for (T* it = m_first + size; it < m_last; ++it)
		{
			it->~T();
		}

		m_last = m_first + size;
	}

	template<typename T>
	SK_FINLINE void Array<T>::Resize(usize size, const T& value)
	{
		Reserve(size);

		for (T* it = m_last; it < m_first + size; ++it)
		{
			new(PlaceHolder(), it) T(value);
		}

		for (T* it = m_first + size; it < m_last; ++it)
		{
			it->~T();
		}

		m_last = m_first + size;
	}

	template<typename T>
	template<typename... Args>
	SK_FINLINE T& Array<T>::EmplaceBack(Args&& ... args)
	{
		if (m_first + 1 > m_capacity)
		{
			Reserve(((m_last - m_first) * 3 / 2));
		}
		m_last = m_first + 1;

		if constexpr (Traits::IsAggregate<T>)
		{
			new(PlaceHolder(), m_last) T{Traits::Forward<Args>(args)...};
		}
		else
		{
			new(PlaceHolder(), m_last) T(Traits::Forward<Args>(args)...);
		}
		return *m_last;
	}

	template<typename T>
	SK_FINLINE void Array<T>::Clear()
	{
		for (T* it = m_first; it < m_last; ++it)
		{
			it->~T();
		}
		m_last = m_first;
	}

	template<typename T>
	Array<T>::~Array()
	{
		if (m_first)
		{
			for (auto it = m_first; it < m_last; ++it)
			{
				it->~T();
			}
			m_allocator->MemFree(m_allocator->alloc, m_first, (usize) ((char*) m_capacity - (char*) m_first));
		}
	}

}