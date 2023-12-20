// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"

namespace Skore::Traits
{

	template<typename... Types>
	using Void = void;

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

	template<typename T>
	constexpr bool IsIntegral = true; //TODO


	template<class T, T... Vals>
	struct IntegerSequence
	{
		static_assert(IsIntegral<T>, "IntegerSequence<T, I...> requires T to be an integral type.");
		using ValueType = T;

		static constexpr usize Size() noexcept
		{
			return sizeof...(Vals);
		}
	};

	template <usize... Vals>
	using IndexSequence = IntegerSequence<usize, Vals...>;

	template <class T, T Size>
	using MakeIntegerSequence = __make_integer_seq<IntegerSequence, T, Size>;

	template<class T, class = void>
	struct AddReference
	{ // add reference (non-referenceable type)
		using Lvalue = T;
		using Rvalue = T;
	};

	template<class T>
	struct AddReference<T, Void<T&>>
	{ // (referenceable type)
		using Lvalue = T&;
		using Rvalue = T&&;
	};


	template<class T>
	using AddRValueReference = typename AddReference<T>::Rvalue;

	template<typename T>
	AddRValueReference<T> Declval() noexcept
	{
		static_assert(AlwaysFalse<T>, "Invalid");
	}

	template<typename Type, typename ... Arguments>
	struct IsDirectConstructibleImpl
	{
		template<typename U, decltype(U{Declval<Arguments>()...})* = nullptr>
		static char Test(int);

		template<typename U>
		static long Test(...);

		static constexpr bool value = sizeof(Test<Type>(0)) == sizeof(char);
	};

	template<typename Type, typename ... Arguments>
	constexpr bool IsDirectConstructible = IsDirectConstructibleImpl<Type, Arguments...>::value;


	template <class T>
	constexpr bool IsDestructible = __is_destructible(T);

	template<typename... Ts>
	struct MakeVoid
	{
		typedef void type;
	};
	template<typename... Ts> using VoidType = typename MakeVoid<Ts...>::type;
}