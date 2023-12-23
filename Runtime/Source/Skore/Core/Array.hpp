// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Traits.hpp"
#include "New.hpp"
#include "Allocator.hpp"

namespace Skore
{

	template<typename T>
	class Array
	{
	public:
		typedef T      * Iterator;
		typedef const T* ConstIterator;


		Array();
		Array(const Array& other);
		Array(Array&& other) noexcept;
		Array(usize size);
		Array(usize size, const T& value);
		Array(const T* first, const T* last);

		Iterator begin();
		Iterator end();
		ConstIterator begin() const;
		ConstIterator end() const;

		Array& operator=(const Array& other);
		Array& operator=(Array&& other) noexcept;
		bool operator==(const Array& other) const;
		bool operator!=(const Array& other) const;
		T& operator[](usize idx);
		const T& operator[](usize idx) const;

		const T* Data() const;
		T* Data();
		const T& Back() const;
		T& Back();
		usize Size() const;
		usize Capacity() const;
		bool Empty() const;


		void Reserve(usize newCapacity);
		void Resize(usize size);
		void Resize(usize size, const T& value);
		void Clear();
		void PopBack();


		void Assign(const T* first, const T* last);
		void Insert(Iterator where, const T* first, const T* last);
		void Erase(Iterator first, Iterator last);

		template<typename ...Args>
		T& EmplaceBack(Args&&... args);


		void ShrinkToFit();
		void Swap(Array& other);


		~Array();
	private:
		T* m_First{};
		T* m_Last{};
		T* m_Capacity{};

		Allocator* m_Allocator = GetDefaultAllocator();
	};

	template<typename T>
	SK_FINLINE Array<T>::Array() : m_First(0), m_Last(0), m_Capacity(0)
	{
	}

	template<typename T>
	SK_FINLINE Array<T>::Array(const Array& other) : m_First(0), m_Last(0), m_Capacity(0), m_Allocator(other.m_Allocator)
	{
		Reserve(other.Size());
		Insert(begin(), other.begin(), other.m_Last);
	}

	template<typename T>
	SK_FINLINE Array<T>::Array(Array&& other) noexcept
	{
		m_First    = other.m_First;
		m_Last           = other.m_Last;
		m_Capacity  = other.m_Capacity;
		m_Allocator = other.m_Allocator;

		other.m_First    = nullptr;
		other.m_Last     = nullptr;
		other.m_Capacity = nullptr;
	}

	template<typename T>
	SK_FINLINE Array<T>::Array(usize size) : m_First(0), m_Last(0), m_Capacity(0)
	{
		Resize(size);
	}

	template<typename T>
	SK_FINLINE Array<T>::Array(usize size, const T& value) : m_First(0), m_Last(0), m_Capacity(0)
	{
		Resize(size, value);
	}

	template<typename T>
	SK_FINLINE Array<T>::Array(const T* first, const T* last) : m_First(0), m_Last(0), m_Capacity(0)
	{
		Insert(begin(), first, last);
	}

	template<typename T>
	SK_FINLINE Array<T>::Iterator Array<T>::begin()
	{
		return m_First;
	}

	template<typename T>
	SK_FINLINE Array<T>::Iterator Array<T>::end()
	{
		return m_Last;
	}

	template<typename T>
	SK_FINLINE Array<T>::ConstIterator Array<T>::begin() const
	{
		return m_First;
	}

	template<typename T>
	SK_FINLINE Array<T>::ConstIterator Array<T>::end() const
	{
		return m_Last;
	}

	template<typename T>
	Array<T>& Array<T>::operator=(const Array& other)
	{
		Array(other).Swap(*this);
		return *this;
	}

	template<typename T>
	Array<T>& Array<T>::operator=(Array&& other) noexcept
	{
		this->~Array();

		m_First    = other.m_First;
		m_Last           = other.m_Last;
		m_Capacity  = other.m_Capacity;
		m_Allocator = other.m_Allocator;

		other.m_First    = nullptr;
		other.m_Last     = nullptr;
		other.m_Capacity = nullptr;

		return *this;
	}

	template<typename T>
	SK_FINLINE T& Array<T>::operator[](usize idx)
	{
		return m_First[idx];
	}

	template<typename T>
	SK_FINLINE const T& Array<T>::operator[](usize idx) const
	{
		return m_First[idx];
	}

	template<typename T>
	bool Array<T>::operator==(const Array& other) const
	{
		if (this->Size() != other.Size()) return false;
		for (int i = 0; i < this->Size(); ++i)
		{
			if (this->operator[](i) != other[i])
			{
				return false;
			}
		}
		return true;
	}

	template<typename T>
	bool Array<T>::operator!=(const Array& other) const
	{
		return !((*this) == other);
	}

	template<typename T>
	SK_FINLINE const T& Array<T>::Back() const
	{
		return m_Last[-1];
	}

	template<typename T>
	SK_FINLINE T& Array<T>::Back()
	{
		return m_Last[-1];
	}

	template<typename T>
	SK_FINLINE const T* Array<T>::Data() const
	{
		return m_First;
	}

	template<typename T>
	SK_FINLINE T* Array<T>::Data()
	{
		return m_First;
	}

	template<typename T>
	SK_FINLINE usize Array<T>::Size() const
	{
		return m_Last - m_First;
	}

	template<typename T>
	SK_FINLINE usize Array<T>::Capacity() const
	{
		return m_Capacity - m_First;
	}

	template<typename T>
	SK_FINLINE bool Array<T>::Empty() const
	{
		return m_Last == m_First;
	}

	template<typename T>
	SK_FINLINE void Array<T>::Reserve(usize newCapacity)
	{
		if (m_First + newCapacity <= m_Capacity)
		{
			return;
		}

		const usize size = m_Last - m_First;
		T* newFirst = (T*) m_Allocator->MemAlloc(m_Allocator->Alloc, sizeof(T) * newCapacity);
		T* dest = newFirst;

		for (T* it = m_First; it != m_Last; ++it, ++dest)
		{
			new(PlaceHolder(), dest) T(Traits::Forward<T>(*it));
			it->~T();
		}

		m_Allocator->MemFree(m_Allocator->Alloc, m_First, sizeof(T) * ((char*) m_Capacity - (char*) m_First));

		m_First    = newFirst;
		m_Last     = newFirst + size;
		m_Capacity = newFirst + newCapacity;
	}

	template<typename T>
	SK_FINLINE void Array<T>::Resize(usize size)
	{
		Reserve(size);

		for (T* it = m_Last; it < m_First + size; ++it)
		{
			new(PlaceHolder(), it) T();
		}

		for (T* it = m_First + size; it < m_Last; ++it)
		{
			it->~T();
		}

		m_Last = m_First + size;
	}

	template<typename T>
	SK_FINLINE void Array<T>::Resize(usize size, const T& value)
	{
		Reserve(size);

		for (T* it = m_Last; it < m_First + size; ++it)
		{
			new(PlaceHolder(), it) T(value);
		}

		for (T* it = m_First + size; it < m_Last; ++it)
		{
			it->~T();
		}

		m_Last = m_First + size;
	}

	template<typename T>
	template<typename... Args>
	SK_FINLINE T& Array<T>::EmplaceBack(Args&& ... args)
	{
		T* where = m_Last;

		if (m_Last == m_Capacity)
		{
			Reserve(((m_Last - m_First) * 3 / 2) + 1);
			where = m_First + (m_Last - m_First);
		}

		m_Last++;

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
		for (T* it = m_First; it < m_Last; ++it)
		{
			it->~T();
		}
		m_Last = m_First;
	}

	template<typename T>
	SK_FINLINE void Array<T>::PopBack()
	{
		T* where = m_Last - 1;
		where->~T();
		--m_Last;
	}

	template<typename T>
	SK_FINLINE void Array<T>::Insert(Array::Iterator where, const T* first, const T* last)
	{
		const usize offset = where - m_First;
		const usize count = last - first;
		const usize newSize = ((m_Last - m_First) + count);
		if (m_First + newSize >= m_Capacity)
		{
			Reserve((newSize * 3) / 2);
			where = m_First + offset;
		}

		T* dest = m_First + newSize - 1;
		for (T* it = m_Last - 1; it >= where; --it, --dest)
		{
			new(PlaceHolder(), dest) T(Traits::Forward<T>(*it));
		}

		for (; first != last; ++first, ++where)
		{
			new(PlaceHolder(), where) T(*first);
		}
		m_Last = m_First + newSize;
	}

	template<typename T>
	void Array<T>::Assign(const T* first, const T* last)
	{
		Clear();
		Insert(m_Last, first, last);
	}

	template<typename T>
	SK_FINLINE void Array<T>::Erase(Array::Iterator first, Array::Iterator last)
	{
		const usize count = (last - first);

		for (T* it = first; it != last; ++it)
		{
			it->~T();
		}

		T* it  = last;
		T* end = m_Last;

		for (T* dest = first; it != end; ++it, ++dest)
		{
			new(PlaceHolder(), dest) T(Traits::Forward<T>(*it));
		}

		m_Last -= count;
	}

	template<typename T>
	SK_FINLINE void Array<T>::ShrinkToFit()
	{
		if (m_Capacity != m_Last)
		{
			if (m_Last == m_First)
			{
				m_Allocator->MemFree(m_Allocator->Alloc, m_First, sizeof(T) * ((char*) m_Capacity - (char*) m_First));
				m_Capacity = m_First = m_Last = nullptr;
			}
			else
			{
				const usize size = m_Last - m_First;
				T* newFirst = (T*) m_Allocator->MemAlloc(m_Allocator->Alloc, sizeof(T) * size);
				T* dest     = newFirst;

				for (T* it = m_First; it != m_Last; ++it, ++dest)
				{
					new(PlaceHolder(), dest) T(Traits::Forward<T>(*it));
					it->~T();
				}

				m_Allocator->MemFree(m_Allocator->Alloc, m_First, sizeof(T) * ((char*) m_Capacity - (char*) m_First));
				m_First    = newFirst;
				m_Last     = newFirst + size;
				m_Capacity = m_Last;
			}
		}
	}

	template<typename T>
	SK_FINLINE void Array<T>::Swap(Array& other)
	{
		T* first    = m_First;
		T* last           = m_Last;
		T* capacity       = m_Capacity;

		Allocator* allocator = m_Allocator;

		m_First    = other.m_First;
		m_Last            = other.m_Last;
		m_Capacity           = other.m_Capacity;
		m_Allocator          = other.m_Allocator;

		other.m_First    = first;
		other.m_Last      = last;
		other.m_Capacity     = capacity;
		other.m_Allocator    = allocator;
	}

	template<typename T>
	Array<T>::~Array()
	{
		if (m_First)
		{
			for (auto it = m_First; it < m_Last; ++it)
			{
				it->~T();
			}
			m_Allocator->MemFree(m_Allocator->Alloc, m_First, sizeof(T) * ((char*) m_Capacity - (char*) m_First));
		}
	}

}