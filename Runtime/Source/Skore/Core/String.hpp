// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Allocator.hpp"

namespace Skore
{
	template<typename Type>
	class BasicStringView;

	template<typename T>
	class BasicString
	{
	public:
		using Type = T;
		using Pointer = T*;
		using ConstPointer = const T*;
		using Iterator = T*;
		using ConstIterator = const T*;

		BasicString();
		BasicString(const BasicString& other);
		BasicString(const BasicStringView<T>& stringView);
		BasicString(BasicString&& other) noexcept;
		BasicString(ConstPointer sz);
		BasicString(ConstPointer first, ConstPointer last);
		BasicString(ConstPointer sz, usize len);
		BasicString(Allocator* allocator);
		BasicString(const BasicStringView<T>& stringView, Allocator* allocator);
		BasicString(ConstPointer sz, Allocator* allocator);
		BasicString(ConstPointer first, ConstPointer last, Allocator* allocator);
		BasicString(ConstPointer sz, usize len, Allocator* allocator);

		~BasicString();

		Iterator begin();
		Iterator end();
		ConstIterator begin() const;
		ConstIterator end() const;

		BasicString& operator=(const BasicString& other);
		BasicString& operator=(BasicString&& other) noexcept;
		BasicString& operator=(Type ch);
		BasicString& operator=(ConstPointer sz);
		BasicString& operator=(const BasicStringView<T>& stringView);
		BasicString& operator+=(const BasicString& other);
		BasicString& operator+=(Type ch);
		BasicString& operator+=(ConstPointer sz);
		Type operator[](usize pos) const;
		Type& operator[](usize pos);

		ConstPointer CStr() const;
		bool Empty() const;
		usize Size() const;
		usize Capacity() const;
		i32 Compare(const BasicString& other) const;
		i32 Compare(ConstPointer sz) const;
		void Reserve(usize capacity);
		void Resize(usize n);
		void SetSize(usize s);
		void Resize(usize n, Type ch);
		void Clear();
		void Assign(Type ch);
		void Assign(ConstPointer sz);
		void Assign(ConstPointer first, ConstPointer last);
		void Assign(const BasicString& other);
		void PushBack(Type ch);
		void Append(ConstPointer sz);
		void Append(Type c);
		void Append(ConstPointer first, ConstPointer last);
		void Append(const BasicString& other);
		void Insert(Iterator where, Type ch);
		void Insert(Iterator where, ConstPointer sz);
		void Insert(Iterator where, ConstPointer first, ConstPointer last);
		void Insert(Iterator where, const BasicString& other);
		void Erase(Iterator first, Iterator last);
		void Swap(BasicString& other);

		template<typename Type>
		BasicString& Append(const Type& value);

	private:
		static constexpr usize c_longFlag = ((usize) 1) << (sizeof(usize) * 8 - 1);

		usize m_size;
		union
		{
			struct
			{
				Pointer m_first;
				Pointer m_capacity;
			};
			Type m_buffer[SK_STRING_BUFFER_SIZE];
		};
		Allocator* m_allocator = GetDefaultAllocator();
	};

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString() : BasicString(GetDefaultAllocator())
	{}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(const BasicStringView<T>& stringView) : BasicString(stringView, GetDefaultAllocator())
	{}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(Allocator* allocator)
		: m_size(0)
	{
		m_buffer[0] = 0;
	}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(const BasicString& other) : m_size(0), m_allocator(other.m_allocator)
	{
		Assign(other);
	}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(const BasicStringView<T>& stringView, Allocator* allocator) : m_size(0), m_allocator(allocator)
	{
		Assign(stringView.begin(), stringView.end());
	}

	template<typename T>
	BasicString<T>::BasicString(BasicString&& other) noexcept : m_size(0)
	{
		m_size      = other.m_size;
		m_allocator = other.m_allocator;
		if (other.m_size & c_longFlag)
		{
			m_capacity = other.m_capacity;
			m_first    = other.m_first;
		}
		else
		{
			for (i32 i             = 0; i < other.Size(); ++i)
			{
				m_buffer[i] = other[i];
			}
			m_buffer[other.Size()] = 0;
		}
		other.m_size = 0;
	}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(ConstPointer sz) : BasicString(sz, GetDefaultAllocator())
	{}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(ConstPointer first, ConstPointer last) : BasicString(first, last, GetDefaultAllocator())
	{}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(ConstPointer sz, usize len) : BasicString(sz, len, GetDefaultAllocator())
	{}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(ConstPointer sz, Allocator* allocator) : m_size(0), m_allocator(allocator)
	{
		Assign(sz);
	}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(ConstPointer first, ConstPointer last, Allocator* allocator) : m_size(0), m_allocator(allocator)
	{
		Assign(first, last);
	}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(ConstPointer sz, usize len, Allocator* allocator) : m_size(0), m_allocator(allocator)
	{
		Assign(sz, sz + len);
	}

	template<typename T>
	SK_FINLINE BasicString<T>::~BasicString()
	{
		if (m_size & c_longFlag)
		{
			m_allocator->MemFree(m_allocator->alloc, m_first, m_capacity - m_first);
		}
	}

	template<typename T>
	SK_FINLINE BasicString<T>& BasicString<T>::operator=(const BasicString& other)
	{
		if (this != &other)
		{
			Assign(other);
		}
		return *this;
	}

	template<typename T>
	SK_FINLINE BasicString<T>& BasicString<T>::operator=(BasicString&& other) noexcept
	{
		m_size = other.m_size;
		if (other.m_size & c_longFlag)
		{
			m_capacity = other.m_capacity;
			m_first    = other.m_first;
		}
		else
		{
			for (int i             = 0; i < other.Size(); ++i)
			{
				m_buffer[i] = other[i];
			}
			m_buffer[other.Size()] = 0;
		}
		other.m_size = 0;
		return *this;
	}

	template<typename T>
	SK_FINLINE BasicString<T>& BasicString<T>::operator=(Type ch)
	{
		Assign(ch);
		return *this;
	}

	template<typename T>
	SK_FINLINE BasicString<T>& BasicString<T>::operator=(ConstPointer sz)
	{
		Assign(sz);
		return *this;
	}

	template<typename T>
	SK_FINLINE BasicString<T>& BasicString<T>::operator=(const BasicStringView<T>& stringView)
	{
		Assign(stringView.begin(), stringView.end());
		return *this;
	}

	template<typename T>
	SK_FINLINE BasicString<T>& BasicString<T>::operator+=(const BasicString& other)
	{
		Append(other);
		return *this;
	}

	template<typename T>
	SK_FINLINE BasicString<T>& BasicString<T>::operator+=(Type ch)
	{
		PushBack(ch);
		return *this;
	}

	template<typename T>
	SK_FINLINE BasicString<T>& BasicString<T>::operator+=(ConstPointer sz)
	{
		Append(sz);
		return *this;
	}

	template<typename T>
	SK_FINLINE typename BasicString<T>::ConstPointer BasicString<T>::CStr() const
	{
		if (m_size & c_longFlag)
		{
			return m_first;
		}
		else
		{
			return m_buffer;
		}
	}

	template<typename T>
	SK_FINLINE bool BasicString<T>::Empty() const
	{
		return Size() == 0;
	}

	template<typename T>
	SK_FINLINE usize BasicString<T>::Size() const
	{
		return m_size & ~c_longFlag;
	}

	template<typename T>
	SK_FINLINE usize BasicString<T>::Capacity() const
	{
		if (m_size & c_longFlag)
		{
			return m_capacity - m_first - 1;
		}
		else
		{
			return SK_STRING_BUFFER_SIZE - 1;
		}
	}

	template<typename T>
	SK_FINLINE typename BasicString<T>::Iterator BasicString<T>::begin()
	{
		if (m_size & c_longFlag)
		{
			return m_first;
		}
		else
		{
			return m_buffer;
		}
	}

	template<typename T>
	SK_FINLINE typename BasicString<T>::Iterator BasicString<T>::end()
	{
		if (m_size & c_longFlag)
		{
			return m_first + (m_size & ~c_longFlag);
		}
		else
		{
			return m_buffer + m_size;
		}
	}

	template<typename T>
	SK_FINLINE typename BasicString<T>::ConstIterator BasicString<T>::begin() const
	{
		if (m_size & c_longFlag)
		{
			return m_first;
		}
		else
		{
			return m_buffer;
		}
	}

	template<typename T>
	SK_FINLINE typename BasicString<T>::ConstIterator BasicString<T>::end() const
	{
		if (m_size & c_longFlag)
		{
			return m_first + (m_size & ~c_longFlag);
		}
		else
		{
			return m_buffer + m_size;
		}
	}

	template<typename T>
	SK_FINLINE typename BasicString<T>::Type BasicString<T>::operator[](usize pos) const
	{
		return begin()[pos];
	}

	template<typename T>
	SK_FINLINE typename BasicString<T>::Type& BasicString<T>::operator[](usize pos)
	{
		return begin()[pos];
	}

	template<typename T>
	SK_FINLINE int BasicString<T>::Compare(const BasicString& other) const
	{
		return Compare(other.CStr());
	}

	template<typename T>
	SK_FINLINE int BasicString<T>::Compare(ConstPointer sz) const
	{
		ConstPointer it = CStr();
		for (; *it && *sz && (*it == *sz); ++it, ++sz);
		return *it - *sz;
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Reserve(usize cap)
	{
		if (cap <= Capacity())
		{
			return;
		}

		auto         newFirst = (Pointer) m_allocator->MemAlloc(m_allocator->alloc, cap + 1);
		for (Pointer it       = begin(), newIt = newFirst, e = end() + 1; it != e; ++it, ++newIt)
		{
			*newIt = *it;
		}

		if (m_size & c_longFlag)
		{
			m_allocator->MemFree(m_allocator->alloc, m_first, m_capacity - m_first);
		}
		else
		{
			m_size |= c_longFlag;
		}

		m_first    = newFirst;
		m_capacity = m_first + cap + 1;
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Resize(usize n)
	{
		Resize(n, 0);
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Resize(usize n, Type ch)
	{
		if (Size() < n)
		{
			Reserve(n);
			for (Pointer it = end(), e = begin() + n; it != e; ++it)
			{
				*it = ch;
			}
		}
		Pointer it = begin() + n;
		*it = 0;
		m_size = n | (m_size & c_longFlag);
	}

	template<typename T>
	void BasicString<T>::SetSize(usize s)
	{
		m_size = s | (m_size & c_longFlag);
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Clear()
	{
		Resize(0);
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Assign(Type ch)
	{
		Pointer it = begin();
		*it       = ch;
		*(it + 1) = 0;
		m_size = 1 | (m_size & c_longFlag);
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Assign(ConstPointer sz)
	{
		usize             len = 0;
		for (ConstPointer it  = sz; *it; ++it)
		{
			++len;
		}

		Assign(sz, sz + len);
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Assign(ConstPointer first, ConstPointer last)
	{
		usize newsize = last - first;
		Reserve(newsize);

		Pointer           newit = begin();
		for (ConstPointer it    = first; it != last; ++it, ++newit)
		{
			*newit = *it;
		}
		*newit                  = 0;
		m_size = newsize | (m_size & c_longFlag);
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Assign(const BasicString& other)
	{
		Assign(other.begin(), other.end());
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::PushBack(Type ch)
	{
		if (Size() != Capacity())
		{
			Pointer it = end();
			*it       = ch;
			*(it + 1) = 0;
			++m_size;
		}
		else
		{
			Append(&ch, &ch + 1);
		}
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Append(ConstPointer sz)
	{
		usize             len = 0;
		for (ConstPointer it  = sz; *it; ++it)
		{
			++len;
		}
		Append(sz, sz + len);
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Append(ConstPointer first, ConstPointer last)
	{
		const usize newsize = (usize) (Size() + (last - first));
		if (newsize > Capacity())
		{
			Reserve((newsize * 3) / 2);
		}

		Pointer           newit = end();
		for (ConstPointer it    = first; it != last; ++it, ++newit)
		{
			*newit = *it;
		}

		*newit = 0;
		m_size = newsize | (m_size & c_longFlag);
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Append(const BasicString& other)
	{
		Append(other.begin(), other.end());
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Append(T c)
	{
		Append(&c, &c + 1);
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Insert(Iterator where, Type ch)
	{
		Insert(where, &ch, &ch + 1);
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Insert(Iterator where, ConstPointer sz)
	{
		usize             len = 0;
		for (ConstPointer it  = sz; *it; ++it)
		{
			++len;
		}
		Insert(where, sz, sz + len);
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Insert(Iterator where, ConstPointer first, ConstPointer last)
	{
		if (first == last)
		{
			return;
		}

		const usize w       = where - begin();
		const usize newSize = (usize) (Size() + (last - first));

		if (newSize > Capacity())
		{
			Reserve((newSize * 3) / 2);
		}

		Pointer      newIt = begin() + newSize;
		for (Pointer it    = end(), e = begin() + w; it >= e; --it, --newIt)
		{
			*newIt = *it;
		}

		newIt = begin() + w;
		for (ConstPointer it = first; it != last; ++it, ++newIt)
		{
			*newIt = *it;
		}

		m_size = newSize | (m_size & c_longFlag);
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Insert(Iterator where, const BasicString& other)
	{
		insert(where, other.begin(), other.end());
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Erase(Iterator first, Iterator last)
	{
		if (first == last)
		{
			return;
		}
		const usize  newSize = (usize) (Size() + (first - last));
		Pointer      newIt   = first;
		for (Pointer it      = last, e = end(); it != e; ++it, ++newIt)
		{
			*newIt = *it;
		}
		*newIt               = 0;
		m_size = newSize | (m_size & c_longFlag);
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Swap(BasicString& other)
	{
		const usize tsize = m_size;
		Pointer     tfirst, tcapacity;
		Type        tbuffer[SK_STRING_BUFFER_SIZE]{};

		if (tsize & c_longFlag)
		{
			tfirst    = m_first;
			tcapacity = m_capacity;
		}
		else
		{
			for (Pointer it = m_buffer, newit = tbuffer, e = m_buffer + tsize + 1; it != e; ++it, ++newit)
			{
				*newit = *it;
			}
		}

		m_size = other.m_size;
		if (other.m_size & c_longFlag)
		{
			m_first    = other.m_first;
			m_capacity = other.m_capacity;
		}
		else
		{
			for (Pointer it = other.m_buffer, newit = m_buffer, e = other.m_buffer + m_size + 1; it != e; ++it, ++newit)
			{
				*newit = *it;
			}
		}

		other.m_size = tsize;
		if (tsize & c_longFlag)
		{
			other.m_first    = tfirst;
			other.m_capacity = tcapacity;
		}
		else
		{
			for (Pointer it = tbuffer, newit = other.m_buffer, e = tbuffer + tsize + 1; it != e; ++it, ++newit)
			{
				*newit = *it;
			}
		}
	}

	template<typename T>
	SK_FINLINE BasicString<T> operator+(const BasicString<T>& lhs, const BasicString<T>& rhs)
	{
		BasicString<T> ret;
		ret.Reserve(lhs.Size() + rhs.Size());
		ret += lhs;
		ret += rhs;
		return ret;
	}

	template<typename T>
	SK_FINLINE BasicString<T> operator+(const BasicString<T>& lhs, typename BasicString<T>::ConstPointer rhs)
	{
		BasicString ret = lhs;
		ret += rhs;
		return ret;
	}

	template<typename T>
	SK_FINLINE BasicString<T> operator+(typename BasicString<T>::ConstPointer lhs, const BasicString<T>& rhs)
	{
		BasicString<T> ret = lhs;
		ret += rhs;
		return ret;
	}

	template<typename T>
	SK_FINLINE BasicString<T> operator+(typename BasicString<T>::ConstPointer lhs, const BasicStringView<T>& rhs)
	{
		BasicString<T> ret = lhs;
		ret += rhs;
		return ret;
	}

	template<typename T>
	SK_FINLINE bool operator==(const BasicString<T>& lhs, const BasicString<T>& rhs)
	{
		if (lhs.Size() != rhs.Size())
		{
			return false;
		}

		return lhs.Compare(rhs) == 0;
	}

	template<typename T>
	SK_FINLINE bool operator==(const BasicString<T>& lhs, const BasicStringView<T>& rhs)
	{
		if (lhs.Size() != rhs.Size())
		{
			return false;
		}

		return lhs.Compare(rhs) == 0;
	}

	template<typename T>
	SK_FINLINE bool operator==(const BasicString<T>& lhs, typename BasicString<T>::ConstPointer rhs)
	{
		return lhs.Compare(rhs) == 0;
	}

	template<typename T>
	SK_FINLINE bool operator==(typename BasicString<T>::ConstPointer lhs, const BasicString<T>& rhs)
	{
		return rhs.Compare(lhs) == 0;
	}

	template<typename T>
	SK_FINLINE bool operator!=(const BasicString<T>& lhs, const BasicString<T>& rhs)
	{
		return !(lhs == rhs);
	}

	template<typename T>
	SK_FINLINE bool operator!=(const BasicString<T>& lhs, typename BasicString<T>::ConstPointer rhs)
	{
		return !(lhs == rhs);
	}

	template<typename T>
	SK_FINLINE bool operator!=(typename BasicString<T>::ConstPointer lhs, const BasicString<T>& rhs)
	{
		return !(lhs == rhs);
	}

	template<typename T>
	SK_FINLINE bool operator!=(const BasicString<T>& lhs, const BasicStringView<T>& rhs)
	{
		return !(lhs == rhs);
	}

	template<typename T>
	SK_FINLINE bool operator<(const BasicString<T>& lhs, const BasicString<T>& rhs)
	{
		return lhs.Compare(rhs) < 0;
	}

	template<typename T>
	SK_FINLINE bool operator<(const BasicString<T>& lhs, typename BasicString<T>::ConstPointer rhs)
	{
		return lhs.Compare(rhs) < 0;
	}

	template<typename T>
	SK_FINLINE bool operator<(typename BasicString<T>::ConstPointer lhs, const BasicString<T>& rhs)
	{
		return rhs.Compare(lhs) > 0;
	}

	template<typename T>
	SK_FINLINE bool operator>(const BasicString<T>& lhs, const BasicString<T>& rhs)
	{
		return rhs < lhs;
	}

	template<typename T>
	SK_FINLINE bool operator>(const BasicString<T>& lhs, typename BasicString<T>::ConstPointer rhs)
	{
		return rhs < lhs;
	}

	template<typename T>
	SK_FINLINE bool operator>(typename BasicString<T>::ConstPointer lhs, const BasicString<T>& rhs)
	{
		return rhs < lhs;
	}

	template<typename T>
	SK_FINLINE bool operator<=(const BasicString<T>& lhs, const BasicString<T>& rhs)
	{
		return !(rhs < lhs);
	}

	template<typename T>
	SK_FINLINE bool operator<=(const BasicString<T>& lhs, typename BasicString<T>::ConstPointer rhs)
	{
		return !(rhs < lhs);
	}

	template<typename T>
	SK_FINLINE bool operator<=(typename BasicString<T>::ConstPointer lhs, const BasicString<T>& rhs)
	{
		return !(rhs < lhs);
	}

	template<typename T>
	SK_FINLINE bool operator>=(const BasicString<T>& lhs, const BasicString<T>& rhs)
	{
		return !(lhs < rhs);
	}

	template<typename T>
	SK_FINLINE bool operator>=(const BasicString<T>& lhs, typename BasicString<T>::ConstPointer rhs)
	{
		return !(lhs < rhs);
	}

	template<typename T>
	SK_FINLINE bool operator>=(typename BasicString<T>::ConstPointer lhs, const BasicString<T>& rhs)
	{
		return !(lhs < rhs);
	}

	template<typename T, typename AppendType>
	SK_FINLINE void operator<<(BasicString<T>& lhs, const AppendType& rhs)
	{
		lhs.Append(rhs);
	}

#ifdef SK_WIN
	using String = BasicString<wchar_t>;
#else
	using String = BasicString<char>;
#endif

}