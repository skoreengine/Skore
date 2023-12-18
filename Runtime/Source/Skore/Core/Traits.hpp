// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

namespace Skore::Traits
{

	template<typename T>
	struct TRemoveReference
	{
		using type = T;
	};

	template<typename T>
	struct TRemoveReference<T&>
	{
		using type = T;
	};

	template<typename T>
	struct TRemoveReference<T&&>
	{
		using type = T;
	};

	template<typename T>
	using RemoveReference = typename TRemoveReference<T>::type;

	template<typename Type>
	struct TRemoveAll
	{
		using type = Type;
	};

	template<typename Type>
	struct TRemoveAll<const Type>
	{
		using type = typename TRemoveAll<Type>::type;
	};

	template<typename Type>
	struct TRemoveAll<Type*>
	{
		using type = typename TRemoveAll<Type>::type;
	};

	template<typename Type>
	struct TRemoveAll<Type&>
	{
		using type = typename TRemoveAll<Type>::type;
	};

	template<typename Type>
	struct TRemoveAll<Type&&>
	{
		using type = typename TRemoveAll<Type>::type;
	};

	template<typename Type>
	using RemoveAll = typename TRemoveAll<Type>::type;


	template<typename T>
	constexpr T&& Forward(RemoveReference<T>& arg)
	{
		return static_cast<T&&>(arg);
	}

	template<typename T>
	constexpr T&& Forward(RemoveReference<T>&& arg)
	{
		return static_cast<T&&>(arg);
	}

	template<typename T>
	constexpr RemoveReference<T>&& Move(T&& arg)
	{
		return static_cast<RemoveReference<T>&&>(arg);
	}

	template<typename T>
	constexpr bool IsAggregate = __is_aggregate(T);

	typedef decltype(__nullptr) NullPtr;


	template<typename T>
	constexpr bool AlwaysFalse = false;
}