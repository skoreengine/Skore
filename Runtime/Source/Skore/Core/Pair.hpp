// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Traits.hpp"


namespace Skore
{
	template<typename Key, typename Value>
	struct Pair
	{
		Key key{};
		Value value{};

		Pair();
		Pair(const Pair& other);
		Pair(Pair&& other);
		Pair(const Key& key, const Value& value);
		Pair(Key&& key, Value&& value);

		Pair& operator=(const Pair& other);
		Pair& operator=(Pair&& other) noexcept;
	};

	template<typename Key, typename Value>
	SK_FINLINE Pair<Key, Value>::Pair()
	{
	}

	template<typename Key, typename Value>
	SK_FINLINE Pair<Key, Value>::Pair(const Pair& other) : key(other.key), value(other.value)
	{

	}

	template<typename Key, typename Value>
	SK_FINLINE Pair<Key, Value>::Pair(Pair&& other) : key(static_cast<Key&&>(other.key)), value(static_cast<Value&&>(other.value))
	{
	}

	template<typename Key, typename Value>
	SK_FINLINE Pair<Key, Value>::Pair(const Key& key, const Value& value) : key(key), value(value)
	{

	}

	template<typename Key, typename Value>
	SK_FINLINE Pair<Key, Value>::Pair(Key&& key, Value&& value) : key(static_cast<Key&&>(key)), value(static_cast<Value&&>(value))
	{
	}

	template<typename Key, typename Value>
	SK_FINLINE Pair<Key, Value>& Pair<Key, Value>::operator=(const Pair& other)
	{
		key   = other.key;
		value = other.value;
		return *this;
	}

	template<typename Key, typename Value>
	SK_FINLINE Pair<Key, Value>& Pair<Key, Value>::operator=(Pair&& other) noexcept
	{
		key   = static_cast<Key&&>(other.key);
		value = static_cast<Value&&>(other.value);
		return *this;
	}

	template<typename Key, typename Value>
	constexpr Pair<Traits::RemoveReference<Key>, Traits::RemoveReference<Value>> MakePair(Key&& key, Value&& value)
	{
		return Pair<Traits::RemoveReference<Key>, Traits::RemoveReference<Value>>(Traits::Forward<Key>(key), Traits::Forward<Value>(value));
	}

	template<typename Key, typename Value>
	constexpr Pair<Traits::RemoveReference<Key>, Traits::RemoveReference<Value>> MakePair(const Key& key, const Value& value)
	{
		return Pair<Traits::RemoveReference<Key>, Traits::RemoveReference<Value>>(key, value);
	}
}