// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"

#include <type_traits>
#include <utility>

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

	typedef decltype(nullptr) NullPtr;


	template<typename T>
	constexpr bool AlwaysFalse = false;

	template<typename T>
	constexpr bool IsIntegral = std::is_integral_v<T>;

    template<class T, T... Vals>
    using IntegerSequence = std::integer_sequence<T, Vals...>;

	template <usize... Vals>
	using IndexSequence = IntegerSequence<usize, Vals...>;

	template <class T, T Size>
	using MakeIntegerSequence = std::make_integer_sequence<T, Size>;

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
	constexpr bool IsDestructible = std::is_destructible_v<T>;

	template<typename... Ts>
	struct MakeVoid
	{
		typedef void type;
	};
	template<typename... Ts> using VoidType = typename MakeVoid<Ts...>::type;


	template<typename Function>
	struct RemoveConstFuncImpl
	{
		using Type = Function;
	};

	template<typename Return, typename Owner, typename ...Args>
	struct RemoveConstFuncImpl<Return(Owner::*)(Args...) const>
	{
		using Type = Return(Owner::*)(Args...);
	};

	template<typename Function>
	using RemoveConstFunc = typename RemoveConstFuncImpl<Function>::Type;

	template<typename T1, typename T2>
	constexpr bool IsSame = std::is_same_v<T1, T2>;

	template<typename T, typename Enabler = void>
	struct IsCompleteImpl : std::false_type
	{
	};

	template<typename T>
	struct IsCompleteImpl<T, Traits::VoidType<decltype(sizeof(T) != 0)>> : std::true_type
	{
	};
	template<typename Type>
	constexpr bool IsComplete = IsCompleteImpl<Type>::value;
}