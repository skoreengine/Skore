#pragma once

#include "Skore/Defines.hpp"
#include "Hash.hpp"

namespace Skore
{

	template<typename Type>
	class BasePath;

	template<typename Type>
	class BasicString;

	template<typename Type>
	class BasicStringView
	{
	public:
		typedef Type ValueType;
		typedef Type* Pointer;
		typedef const Type* ConstPointer;
		typedef ConstPointer Iterator;
		typedef ConstPointer ConstIterator;

		static constexpr usize npos = usize(-1);

		constexpr BasicStringView();
		constexpr BasicStringView(const Type* s, usize count);
		constexpr BasicStringView(const Type* s);
		constexpr BasicStringView(const BasicString<Type>& string);
		constexpr BasicStringView(const BasePath<Type>& path);
		constexpr BasicStringView(const BasicStringView&) = default;

		BasicStringView& operator=(const BasicStringView&) = default;

		constexpr const Type* Data() const;
		constexpr ConstPointer CStr() const;

		constexpr char operator[](usize pos) const;

		constexpr usize Size() const;

		constexpr bool Empty() const;

		constexpr Iterator begin() const;

		constexpr ConstIterator cbegin() const;

		constexpr Iterator end() const;

		constexpr ConstIterator cend() const;

		constexpr BasicStringView Substr(usize pos = 0, usize count = npos) const;

		constexpr void Swap(BasicStringView& v);

		constexpr i32 Compare(const BasicStringView& other) const;

		constexpr i32 Compare(ConstPointer sz) const;

		static constexpr usize Strlen(const Type*);

	private:
		BasicStringView(decltype(nullptr)) = delete;

		const Type* m_Str;
		usize m_Size;
	};

	template<typename Type>
	constexpr BasicStringView<Type>::BasicStringView(const BasePath<Type>& path) : m_Str(path.begin()), m_Size(path.end() - path.begin())
	{
	}

	template<typename Type>
	constexpr BasicStringView<Type>::BasicStringView(const BasicString<Type>& string) : m_Str(string.CStr()), m_Size(string.Size())
	{
	}

	template<typename Type>
	constexpr BasicStringView<Type>::BasicStringView() : m_Str(nullptr), m_Size(0)
	{
	}

	template<typename Type>
	constexpr BasicStringView<Type>::BasicStringView(const Type* s, usize count) : m_Str(s), m_Size(count)
	{
	}

	template<typename Type>
	constexpr BasicStringView<Type>::BasicStringView(const Type* s) : m_Str(s), m_Size(Strlen(s))
	{
	}

	template<typename Type>
	constexpr const Type* BasicStringView<Type>::Data() const
	{
		return m_Str;
	}

	template<typename Type>
	constexpr typename BasicStringView<Type>::ConstPointer BasicStringView<Type>::CStr() const
	{
		return m_Str;
	}

	template<typename Type>
	constexpr char BasicStringView<Type>::operator[](usize pos) const
	{
		return m_Str[pos];
	}

	template<typename Type>
	constexpr usize BasicStringView<Type>::Size() const
	{
		return m_Size;
	}

	template<typename Type>
	constexpr bool BasicStringView<Type>::Empty() const
	{
		return 0 == m_Size;
	}

	template<typename Type>
	constexpr typename BasicStringView<Type>::Iterator BasicStringView<Type>::begin() const
	{
		return m_Str;
	}

	template<typename Type>
	constexpr typename BasicStringView<Type>::ConstIterator BasicStringView<Type>::cbegin() const
	{
		return m_Str;
	}

	template<typename Type>
	constexpr typename BasicStringView<Type>::Iterator BasicStringView<Type>::end() const
	{
		return m_Str + m_Size;
	}

	template<typename Type>
	constexpr typename BasicStringView<Type>::ConstIterator BasicStringView<Type>::cend() const
	{
		return m_Str + m_Size;
	}

	template<typename Type>
	constexpr BasicStringView<Type> BasicStringView<Type>::Substr(usize pos, usize count) const
	{
		return BasicStringView(m_Str + pos, npos == count ? m_Size - pos : count);
	}

	template<typename Type>
	constexpr void BasicStringView<Type>::Swap(BasicStringView<Type>& v)
	{
		Type* strtmp = m_Str;
		usize sizetmp = m_Size;
		m_Str = v.m_Str;
		m_Size = v.m_Size;
		v.m_Str = strtmp;
		v.m_Size = sizetmp;
	}

	template<typename Type>
	constexpr usize BasicStringView<Type>::Strlen(const Type* s)
	{
		for (usize len = 0;; ++len)
		{
			if (0 == s[len])
			{
				return len;
			}
		}
	}

	template<typename Type>
	constexpr i32 BasicStringView<Type>::Compare(const BasicStringView<Type>& other) const
	{
		if (this->Size() != other.Size())
		{
			return false;
		}
		int ctr = 0;
		while (this->m_Str[ctr] == other.m_Str[ctr])
		{
			if (this->Size() == ctr)
			{
				return 0;
			}
			ctr++;
		}
		if (this->Size() == ctr)
		{
			return 0;
		}
		return -1;
	}

	template<typename Type>
	constexpr i32 BasicStringView<Type>::Compare(typename BasicStringView<Type>::ConstPointer sz) const
	{
		ConstPointer it = CStr();
		if (!it) return -1;
		for (; *it && *sz && (*it == *sz); ++it, ++sz);
		return *it - *sz;
	}

	template<typename Type>
	SK_FINLINE bool operator==(const BasicStringView<Type>& lhs, const BasicStringView<Type>& rhs)
	{
		if (lhs.Size() != rhs.Size())
		{
			return false;
		}

		return lhs.Compare(rhs) == 0;
	}

	template<typename Type>
	SK_FINLINE bool operator==(const BasicStringView<Type>& lhs, typename BasicStringView<Type>::ConstPointer rhs)
	{
		return lhs.Compare(rhs) == 0;
	}

	template<typename Type>
	SK_FINLINE bool operator==(typename BasicStringView<Type>::ConstPointer lhs, const BasicStringView<Type>& rhs)
	{
		return rhs.Compare(lhs) == 0;
	}

	template<typename Type>
	SK_FINLINE bool operator!=(const BasicStringView<Type>& lhs, const BasicStringView<Type>& rhs)
	{
		return !(lhs == rhs);
	}

	template<typename Type>
	SK_FINLINE bool operator!=(const BasicStringView<Type>& lhs, typename BasicStringView<Type>::ConstPointer rhs)
	{
		return !(lhs == rhs);
	}

	template<typename Type>
	SK_FINLINE bool operator!=(typename BasicStringView<Type>::ConstPointer lhs, const BasicStringView<Type>& rhs)
	{
		return !(lhs == rhs);
	}

	template<typename Type>
	SK_FINLINE bool operator<(const BasicStringView<Type>& lhs, const BasicStringView<Type>& rhs)
	{
		return lhs.Compare(rhs) < 0;
	}

	template<typename Type>
	SK_FINLINE bool operator<(const BasicStringView<Type>& lhs, typename BasicStringView<Type>::ConstPointer rhs)
	{
		return lhs.Compare(rhs) < 0;
	}

	template<typename Type>
	SK_FINLINE bool operator<(typename BasicStringView<Type>::ConstPointer lhs, const BasicStringView<Type>& rhs)
	{
		return rhs.Compare(lhs) > 0;
	}

	template<typename Type>
	SK_FINLINE bool operator>(const BasicStringView<Type>& lhs, const BasicStringView<Type>& rhs)
	{
		return rhs < lhs;
	}

	template<typename Type>
	SK_FINLINE bool operator>(const BasicStringView<Type>& lhs, typename BasicStringView<Type>::ConstPointer rhs)
	{
		return rhs < lhs;
	}

	template<typename Type>
	SK_FINLINE bool operator>(typename BasicStringView<Type>::ConstPointer lhs, const BasicStringView<Type>& rhs)
	{
		return rhs < lhs;
	}

	template<typename Type>
	SK_FINLINE bool operator<=(const BasicStringView<Type>& lhs, const BasicStringView<Type>& rhs)
	{
		return !(rhs < lhs);
	}

	template<typename Type>
	SK_FINLINE bool operator<=(const BasicStringView<Type>& lhs, typename BasicStringView<Type>::ConstPointer rhs)
	{
		return !(rhs < lhs);
	}

	template<typename Type>
	SK_FINLINE bool operator<=(typename BasicStringView<Type>::ConstPointer lhs, const BasicStringView<Type>& rhs)
	{
		return !(rhs < lhs);
	}

	template<typename Type>
	SK_FINLINE bool operator>=(const BasicStringView<Type>& lhs, const BasicStringView<Type>& rhs)
	{
		return !(lhs < rhs);
	}

	template<typename Type>
	SK_FINLINE bool operator>=(const BasicStringView<Type>& lhs, typename BasicStringView<Type>::ConstPointer rhs)
	{
		return !(lhs < rhs);
	}

	template<typename Type>
	SK_FINLINE bool operator>=(typename BasicStringView<Type>::ConstPointer lhs, const BasicStringView<Type>& rhs)
	{
		return !(lhs < rhs);
	}

	using StringView = BasicStringView<char>;


	template<typename Element>
	struct Hash<BasicStringView<Element>>
	{
		constexpr static bool HasHash = true;
		constexpr static usize Value(const BasicStringView<Element>& string)
		{
			usize hash = 0;
			for (auto it = string.begin(); it != string.end(); ++it)
			{
				hash = *it + (hash << 6) + (hash << 16) - hash;
			}
			return hash;
		}
	};
}