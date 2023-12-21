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


		Span() : m_first(0), m_last(0)
		{}


		Span(Array<T>& vec) : m_first(vec.Data()), m_last(vec.Data() + vec.Size())
		{
		}

		Span(const Array<T>& vec) : m_first((T*)vec.Data()), m_last((T*)vec.Data() + vec.Size())
		{
		}

		Span(T* t) : m_first(t), m_last(t + 1)
		{
		}

		Span(T* m_First, T* m_Last) : m_first(m_First), m_last(m_Last)
		{}

		constexpr const T* Data() const
		{
			return begin();
		}

		constexpr Iterator begin()
		{
			return m_first;
		}

		constexpr Iterator end()
		{
			return m_last;
		}

		constexpr ConstIterator begin() const
		{
			return m_first;
		}

		constexpr ConstIterator end() const
		{
			return m_last;
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
		T* m_first;
		T* m_last;
	};

}