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
		typedef T      * Iterator;
		typedef const T* ConstIterator;


		Array();
//		Array(const Array& other);
//		Array(Array&& other);
//		Array(usize size);
//		Array(usize size, const T& value);
//		Array(const T* first, const T* last);

		Iterator begin();
		Iterator end();
		ConstIterator begin() const;
		ConstIterator end() const;


		void Reserve(usize size);
		void Resize(usize size);
		void Resize(usize size, const T& value);
		void Clear();
		void PopBack();

		void Insert(Iterator where, const T* first, const T* last);

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
	SK_FINLINE Array<T>::Array() : m_first(0), m_last(0), m_capacity(0)
	{
	}

	template<typename T>
	SK_FINLINE Array<T>::Iterator Array<T>::begin()
	{
		return m_first;
	}

	template<typename T>
	SK_FINLINE Array<T>::Iterator Array<T>::end()
	{
		return m_last;
	}

	template<typename T>
	SK_FINLINE Array<T>::ConstIterator Array<T>::begin() const
	{
		return m_first;
	}

	template<typename T>
	SK_FINLINE Array<T>::ConstIterator Array<T>::end() const
	{
		return m_last;
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
		T* dest = newFirst;

		for (T* it = m_first; it != m_last; ++it, ++dest)
		{
			new(PlaceHolder(), dest) T(Traits::Forward<T>(*it));
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
		T* where = m_last;

		if (m_last == m_capacity)
		{
			Reserve(((m_last - m_first) * 3 / 2) + 1);
			where = m_first + (m_last - m_first);
		}

		m_last++;

		if constexpr (Traits::IsAggregate<T>)
		{
			new(PlaceHolder(), where) T{Traits::Forward<Args>(args)...};
		}
		else
		{
			new(PlaceHolder(), where) T(Traits::Forward<Args>(args)...);
		}
		return *where;
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
	SK_FINLINE void Array<T>::PopBack()
	{
		T* where = m_last - 1;
		where->~T();
		--m_last;
	}

	template<typename T>
	SK_FINLINE void Array<T>::Insert(Array::Iterator where, const T* first, const T* last)
	{
		const usize count = last - first;
		const usize newSize = ((m_last - m_first) + count);
		if (m_first + newSize >= m_capacity)
		{
			Reserve((newSize * 3) / 2);
		}




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