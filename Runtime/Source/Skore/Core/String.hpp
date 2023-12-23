// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Allocator.hpp"
#include "Hash.hpp"
#include "StringConverter.hpp"

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
		BasicString(usize size);
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
		BasicString<T>& Append(const Type& value)
		{
			static_assert(StringConverter<Type>::HasConverter, "[StringConverter] type has no converter");
			if constexpr (StringConverter<Type>::BufferCount > 0)
			{
				char buffer[StringConverter<Type>::BufferCount];
				auto size = StringConverter<Type>::ToString(buffer, 0, value);
				Append(buffer, buffer + size);
			}
			else
			{
				auto initialSize = Size();
				auto newSize = Size() + StringConverter<Type>::Size(value);
				Resize(newSize);
				StringConverter<Type>::ToString(begin() + initialSize, 0, value);
				m_Size = newSize | (m_Size & LongFlag);
			}
			return *this;
		}

	private:
		static constexpr usize LongFlag = ((usize) 1) << (sizeof(usize) * 8 - 1);

		usize m_Size;
		union
		{
			struct
			{
				Pointer m_First;
				Pointer m_Capacity;
			};
			Type m_Buffer[SK_STRING_BUFFER_SIZE];
		};
		Allocator* m_Allocator = GetDefaultAllocator();
	};

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString() : BasicString(GetDefaultAllocator())
	{}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(const BasicStringView<T>& stringView) : BasicString(stringView, GetDefaultAllocator())
	{}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(Allocator* allocator) : m_Size(0), m_Allocator(allocator)
	{
		m_Buffer[0] = 0;
	}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(usize size)
	{
		Resize(size);
	}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(const BasicString& other) : m_Size(0), m_Allocator(other.m_Allocator)
	{
		Assign(other);
	}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(const BasicStringView<T>& stringView, Allocator* allocator) : m_Size(0), m_Allocator(allocator)
	{
		Assign(stringView.begin(), stringView.end());
	}

	template<typename T>
	BasicString<T>::BasicString(BasicString&& other) noexcept : m_Size(0)
	{
		m_Size      = other.m_Size;
		m_Allocator = other.m_Allocator;
		if (other.m_Size & LongFlag)
		{
			m_Capacity = other.m_Capacity;
			m_First    = other.m_First;
		}
		else
		{
			for (i32 i             = 0; i < other.Size(); ++i)
			{
				m_Buffer[i] = other[i];
			}
			m_Buffer[other.Size()] = 0;
		}
		other.m_Size = 0;
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
	SK_FINLINE BasicString<T>::BasicString(ConstPointer sz, Allocator* allocator) : m_Size(0), m_Allocator(allocator)
	{
		Assign(sz);
	}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(ConstPointer first, ConstPointer last, Allocator* allocator) : m_Size(0), m_Allocator(allocator)
	{
		Assign(first, last);
	}

	template<typename T>
	SK_FINLINE BasicString<T>::BasicString(ConstPointer sz, usize len, Allocator* allocator) : m_Size(0), m_Allocator(allocator)
	{
		Assign(sz, sz + len);
	}

	template<typename T>
	SK_FINLINE BasicString<T>::~BasicString()
	{
		if (m_Size & LongFlag)
		{
			m_Allocator->MemFree(m_Allocator->Alloc, m_First, m_Capacity - m_First);
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
		m_Size = other.m_Size;

		if (other.m_Size & LongFlag)
		{
			m_Capacity = other.m_Capacity;
			m_First    = other.m_First;
		}
		else
		{
			for (int i             = 0; i < other.Size(); ++i)
			{
				m_Buffer[i] = other[i];
			}
			m_Buffer[other.Size()] = 0;
		}
		other.m_Size = 0;
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
		if (m_Size & LongFlag)
		{
			return m_First;
		}
		else
		{
			return m_Buffer;
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
		return m_Size & ~LongFlag;
	}

	template<typename T>
	SK_FINLINE usize BasicString<T>::Capacity() const
	{
		if (m_Size & LongFlag)
		{
			return m_Capacity - m_First - 1;
		}
		else
		{
			return SK_STRING_BUFFER_SIZE - 1;
		}
	}

	template<typename T>
	SK_FINLINE typename BasicString<T>::Iterator BasicString<T>::begin()
	{
		if (m_Size & LongFlag)
		{
			return m_First;
		}
		else
		{
			return m_Buffer;
		}
	}

	template<typename T>
	SK_FINLINE typename BasicString<T>::Iterator BasicString<T>::end()
	{
		if (m_Size & LongFlag)
		{
			return m_First + (m_Size & ~LongFlag);
		}
		else
		{
			return m_Buffer + m_Size;
		}
	}

	template<typename T>
	SK_FINLINE typename BasicString<T>::ConstIterator BasicString<T>::begin() const
	{
		if (m_Size & LongFlag)
		{
			return m_First;
		}
		else
		{
			return m_Buffer;
		}
	}

	template<typename T>
	SK_FINLINE typename BasicString<T>::ConstIterator BasicString<T>::end() const
	{
		if (m_Size & LongFlag)
		{
			return m_First + (m_Size & ~LongFlag);
		}
		else
		{
			return m_Buffer + m_Size;
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

		auto         newFirst = (Pointer) m_Allocator->MemAlloc(m_Allocator->Alloc, cap + 1);
		for (Pointer it       = begin(), newIt = newFirst, e = end() + 1; it != e; ++it, ++newIt)
		{
			*newIt = *it;
		}

		if (m_Size & LongFlag)
		{
			m_Allocator->MemFree(m_Allocator->Alloc, m_First, m_Capacity - m_First);
		}
		else
		{
			m_Size |= LongFlag;
		}

		m_First    = newFirst;
		m_Capacity = m_First + cap + 1;
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
		m_Size = n | (m_Size & LongFlag);
	}

	template<typename T>
	void BasicString<T>::SetSize(usize s)
	{
		m_Size = s | (m_Size & LongFlag);
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
		m_Size = 1 | (m_Size & LongFlag);
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
		m_Size = newsize | (m_Size & LongFlag);
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
			++m_Size;
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
		m_Size = newsize | (m_Size & LongFlag);
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

		m_Size = newSize | (m_Size & LongFlag);
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
		m_Size = newSize | (m_Size & LongFlag);
	}

	template<typename T>
	SK_FINLINE void BasicString<T>::Swap(BasicString& other)
	{
		const usize tsize = m_Size;
		Pointer     tfirst, tcapacity;
		Type        tbuffer[SK_STRING_BUFFER_SIZE]{};

		if (tsize & LongFlag)
		{
			tfirst    = m_First;
			tcapacity = m_Capacity;
		}
		else
		{
			for (Pointer it = m_Buffer, newit = tbuffer, e = m_Buffer + tsize + 1; it != e; ++it, ++newit)
			{
				*newit = *it;
			}
		}

		m_Size = other.m_Size;
		if (other.m_Size & LongFlag)
		{
			m_First    = other.m_First;
			m_Capacity = other.m_Capacity;
		}
		else
		{
			for (Pointer it = other.m_Buffer, newit = m_Buffer, e = other.m_Buffer + m_Size + 1; it != e; ++it, ++newit)
			{
				*newit = *it;
			}
		}

		other.m_Size = tsize;
		if (tsize & LongFlag)
		{
			other.m_First    = tfirst;
			other.m_Capacity = tcapacity;
		}
		else
		{
			for (Pointer it = tbuffer, newit = other.m_Buffer, e = tbuffer + tsize + 1; it != e; ++it, ++newit)
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

	template<typename Element>
	struct Hash<BasicString<Element>>
	{
		constexpr static bool HasHash = true;

		constexpr static usize Value(const BasicStringView<Element>& stringView)
		{
			usize hash = 0;
			for (const char c: stringView)
			{
				hash = c + (hash << 6) + (hash << 16) - hash;
			}
			return hash;
		}

		constexpr static usize Value(const char* ch)
		{
			usize hash = 0;
			for (const char* c = ch; *c != '\0'; ++c)
			{
				hash = *ch + (hash << 6) + (hash << 16) - hash;
			}
			return hash;
		}

		constexpr static usize Value(const BasicString<Element>& string)
		{
			usize hash = 0;
			for (auto it = string.begin(); it != string.end(); ++it)
			{
				hash = *it + (hash << 6) + (hash << 16) - hash;
			}
			return hash;
		}
	};

	using String = BasicString<char>;




}