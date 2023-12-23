//  Copyright 2023 Paulo Marangoni.
//  Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"

namespace Skore
{

	template<typename T>
	class Array;

	template<typename T>
	class Span
	{
	public:
		typedef T ValueType;
		typedef T* Iterator;
		typedef const T* ConstIterator;


		Span() : m_First(0), m_Last(0)
		{}


		Span(Array<T>& vec) : m_First(vec.Data()), m_Last(vec.Data() + vec.Size())
		{
		}

		Span(const Array<T>& vec) : m_First((T*)vec.Data()), m_Last((T*)vec.Data() + vec.Size())
		{
		}

		Span(T* t) : m_First(t), m_Last(t + 1)
		{
		}

		Span(T* first, T* last) : m_First(first), m_Last(last)
		{}

		Span(T* first, usize size) : m_First(first), m_Last(first + size)
		{}

		constexpr const T* Data() const
		{
			return begin();
		}

		constexpr Iterator begin()
		{
			return m_First;
		}

		constexpr Iterator end()
		{
			return m_Last;
		}

		constexpr ConstIterator begin() const
		{
			return m_First;
		}

		constexpr ConstIterator end() const
		{
			return m_Last;
		}

		constexpr usize Size() const
		{
			return (usize) (end() - begin());
		}

		constexpr bool Empty() const
		{
			return Size() == 0;
		}

		constexpr T& operator[](usize idx)
		{
			return begin()[idx];
		}

		constexpr const T& operator[](usize idx) const
		{
			return begin()[idx];
		}

		constexpr const T& Back() const
		{
			return begin()[Size() - 1];
		}

		constexpr T& Back()
		{
			return begin()[Size() - 1];
		}

	private:
		T* m_First;
		T* m_Last;
	};

}