// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Hash.hpp"
#include "HashBase.hpp"
#include "Pair.hpp"
#include "Allocator.hpp"
#include "Array.hpp"
#include "Traits.hpp"

namespace Skore
{
	template<typename Key, typename Value>
	class HashMap
	{
	public:
		typedef Pair<Key, Value>         ValueType;
		typedef HashNode<Key, Value>     Node;
		typedef HashIterator<Node>       Iterator;
		typedef HashIterator<const Node> ConstIterator;

		Iterator begin();
		Iterator end();
		ConstIterator begin() const;
		ConstIterator end() const;

		void    Clear();
		bool    Empty() const;
		usize   Size() const;

		template<typename ParamKey>
		Iterator Find(const ParamKey& key);

		Pair<Iterator, bool> Insert(const Pair<Key, Value>& p);

	private:
		void ReHash();

		usize        m_size{};
		Array<Node*> m_buckets{};
		Allocator* m_allocator = GetDefaultAllocator();
	};

	template<typename Key, typename Value>
	SK_FINLINE void HashMap<Key, Value>::Clear()
	{
		if (m_buckets.Empty()) return;

		Node* it = *m_buckets.begin();
		while (it)
		{
			Node* next = it->next;
			it->~HashNode<Key, Value>();
			m_allocator->MemFree(m_allocator->alloc, it, sizeof(HashNode<Key, Value>));
			it = next;
		}

		m_buckets.Clear();
		m_buckets.ShrinkToFit();
		m_size = 0;
	}

	template<typename Key, typename Value>
	SK_FINLINE HashMap<Key, Value>::Iterator HashMap<Key, Value>::begin()
	{
		Iterator it;
		it.node = !m_buckets.Empty() ? *m_buckets.begin() : nullptr;
		return it;
	}

	template<typename Key, typename Value>
	SK_FINLINE HashMap<Key, Value>::Iterator HashMap<Key, Value>::end()
	{
		Iterator it;
		it.node = nullptr;
		return it;
	}

	template<typename Key, typename Value>
	SK_FINLINE HashMap<Key, Value>::ConstIterator HashMap<Key, Value>::begin() const
	{
		Iterator it;
		it.node = !m_buckets.Empty() ? *m_buckets.begin() : nullptr;
		return it;
	}

	template<typename Key, typename Value>
	SK_FINLINE HashMap<Key, Value>::ConstIterator HashMap<Key, Value>::end() const
	{
		Iterator it;
		it.node = nullptr;
		return it;
	}

	template<typename Key, typename Value>
	SK_FINLINE bool HashMap<Key, Value>::Empty() const
	{
		return m_size == 0;
	}

	template<typename Key, typename Value>
	SK_FINLINE usize HashMap<Key, Value>::Size() const
	{
		return m_size;
	}

	template<typename Key, typename Value>
	template<typename ParamKey>
	SK_FINLINE HashMap<Key, Value>::Iterator HashMap<Key, Value>::Find(const ParamKey& key)
	{
		if (m_buckets.Empty()) return {};

		const usize bucket = Hash<Key>::Value(key) & (m_buckets.Size() - 2);

		typedef Node* NodePtr;
		for (NodePtr it = m_buckets[bucket], end = m_buckets[bucket + 1]; it != end; it = it->next)
		{
			if (it->first == key)
			{
				return {it};
			}
		}

		return {};
	}

	template<typename Key, typename Value>
	SK_FINLINE Pair<typename HashMap<Key, Value>::Iterator, bool> HashMap<Key, Value>::Insert(const Pair<Key, Value>& p)
	{
		Pair<Iterator, bool> result{};
		result.second = false;
		result.first  = Find(p.first);
		if (result.first.node != nullptr)
		{
			return result;
		}

		Node* newNode = new(PlaceHolder(), m_allocator->MemAlloc(m_allocator->alloc, sizeof(Node))) Node{p.first, p.second};
		newNode->next = newNode->prev = nullptr;

		if (m_buckets.Empty())
		{
			m_buckets.Resize(9);
		}

		HashNodeInsert(newNode, Hash<Key>::Value(p.first), m_buckets.Data(), m_buckets.Size() - 1);

		++m_size;

		ReHash();

		result.first.node = newNode;
		result.second = true;
		return result;
	}

	template<typename Key, typename Value>
	SK_FINLINE void HashMap<Key, Value>::ReHash()
	{
		if (m_size + 1 > 4 * m_buckets.Size())
		{
			Node* root = *m_buckets.begin();
			const usize newNumberBuckets = (m_buckets.Size() - 1) * 8;

			m_buckets.Clear();
			m_buckets.Resize(newNumberBuckets + 1);

			while (root)
			{
				Node*  next = root->next;
				root->next = root->prev = 0;
				HashNodeInsert(root, Hash<Key>::Value(root->first), m_buckets.Data(), m_buckets.Size() - 1);
				root = next;
			}
		}
	}
}