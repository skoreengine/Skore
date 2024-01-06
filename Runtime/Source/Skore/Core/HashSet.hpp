// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Pair.hpp"
#include "HashBase.hpp"
#include "Array.hpp"
#include "Hash.hpp"
namespace Skore
{
	template<typename Key>
	class HashSet
	{
	public:
		typedef Pair<Key, void>          ValueType;
		typedef HashNode<Key, void>      Node;
		typedef HashIterator<Node>       Iterator;
		typedef HashIterator<const Node> ConstIterator;

		HashSet();
		HashSet(const HashSet& other);
		HashSet(HashSet&& other) noexcept;

		Iterator begin();
		Iterator end();
		ConstIterator begin() const;
		ConstIterator end() const;

		void    Clear();
		bool    Empty() const;
		usize   Size() const;

		template<typename ParamKey>
		Iterator Find(const ParamKey& key);

		template<typename ParamKey>
		ConstIterator Find(const ParamKey& key) const;

		void Erase(ConstIterator where);
		void Erase(Iterator where);

		template<typename ParamKey>
		void Erase(const ParamKey& key);

		Pair<Iterator, bool> Insert(const Key& key);
		Pair<Iterator, bool> Emplace(const Key& key);

		template<typename ParamKey>
		bool Has(const ParamKey& key) const;

		~HashSet();


	private:
		void ReHash();

		usize        m_Size{};
		Array<Node*> m_Buckets{};
		AllocatorPtr m_Allocator = GetDefaultAllocator();
	};

	template<typename Key>
	HashSet<Key>::HashSet()
	{

	}

	template<typename Key>
	HashSet<Key>::HashSet(const HashSet& other) : m_Size(other.m_Size)
	{
		if (other.m_Buckets.Empty()) return;
		m_Buckets.Resize(other.m_Buckets.Size());

		for (Node* it = *other.m_Buckets.begin(); it != nullptr; it = it->Next)
		{
			Node* newNode = new(PlaceHolder(), m_Allocator->MemAlloc(m_Allocator->Alloc, sizeof(Node))) Node{it->First};
			newNode->Next = newNode->Prev = nullptr;
			HashNodeInsert(newNode, Hash<Key>::Value(it->First), m_Buckets.Data(), m_Buckets.Size() - 1);
		}
	}

	template<typename Key>
	HashSet<Key>::HashSet(HashSet&& other) noexcept
	{
		m_Buckets.Swap(other.m_Buckets);
		other.m_Size = 0;
	}

	template<typename Key>
	HashSet<Key>::Iterator HashSet<Key>::begin()
	{
		Iterator it;
		it.Node = !m_Buckets.Empty() ? *m_Buckets.begin() : nullptr;
		return it;
	}

	template<typename Key>
	HashSet<Key>::Iterator HashSet<Key>::end()
	{
		Iterator it;
		it.Node = nullptr;
		return it;
	}

	template<typename Key>
	HashSet<Key>::ConstIterator HashSet<Key>::begin() const
	{
		Iterator it;
		it.Node = !m_Buckets.Empty() ? *m_Buckets.begin() : nullptr;
		return it;
	}

	template<typename Key>
	HashSet<Key>::ConstIterator HashSet<Key>::end() const
	{
		Iterator it;
		it.Node = nullptr;
		return it;
	}

	template<typename Key>
	void HashSet<Key>::Clear()
	{
		if (m_Buckets.Empty()) return;

		Node* it = *m_Buckets.begin();
		while (it)
		{
			Node* next = it->Next;
			it->~HashNode<Key, void>();
			m_Allocator->MemFree(m_Allocator->Alloc, it, sizeof(HashNode<Key, void>));
			it = next;
		}

		m_Buckets.Clear();
		m_Buckets.ShrinkToFit();
		m_Size = 0;
	}

	template<typename Key>
	bool HashSet<Key>::Empty() const
	{
		return m_Size == 0;
	}

	template<typename Key>
	usize HashSet<Key>::Size() const
	{
		return m_Size;
	}

	template<typename Key>
	template<typename ParamKey>
	HashSet<Key>::Iterator HashSet<Key>::Find(const ParamKey& key)
	{
		if (m_Buckets.Empty()) return {};

		const usize bucket = Hash<Key>::Value(key) & (m_Buckets.Size() - 2);

		typedef Node* NodePtr;
		for (NodePtr it = m_Buckets[bucket], end = m_Buckets[bucket + 1]; it != end; it = it->Next)
		{
			if (it->First == key)
			{
				return {it};
			}
		}

		return {};
	}

	template<typename Key>
	template<typename ParamKey>
	HashSet<Key>::ConstIterator HashSet<Key>::Find(const ParamKey& key) const
	{
		if (m_Buckets.Empty()) return {};

		const usize bucket = Hash<Key>::Value(key) & (m_Buckets.Size() - 2);

		typedef Node* NodePtr;
		for (NodePtr it = m_Buckets[bucket], end = m_Buckets[bucket + 1]; it != end; it = it->Next)
		{
			if (it->First == key)
			{
				return {it};
			}
		}

		return {};
	}

	template<typename Key>
	void HashSet<Key>::Erase(HashSet::ConstIterator where)
	{
		HashNodeErase(where.Node, Hash<Key>::Value(where->First), m_Buckets.Data(), m_Buckets.Size() - 1);
		where->~HashNode();
		m_Allocator->MemFree(m_Allocator->Alloc, where.Node, sizeof(HashNode<Key, void>));
		--m_Size;
	}

	template<typename Key>
	void HashSet<Key>::Erase(HashSet::Iterator where)
	{
		HashNodeErase(where.Node, Hash<Key>::Value(where->First), m_Buckets.Data(), m_Buckets.Size() - 1);
		where->~HashNode();
		m_Allocator->MemFree(m_Allocator->Alloc, where.Node, sizeof(HashNode<Key, void>));
		--m_Size;
	}

	template<typename Key>
	template<typename ParamKey>
	void HashSet<Key>::Erase(const ParamKey& key)
	{
		Iterator it = Find(key);
		if (it)
		{
			Erase(it);
		}
	}

	template<typename Key>
	Pair<typename HashSet<Key>::Iterator, bool> HashSet<Key>::Insert(const Key& key)
	{
		Pair<Iterator, bool> result{};
		result.Second = false;
		result.First  = Find(key);
		if (result.First.Node != nullptr)
		{
			return result;
		}

		Node* newNode = new(PlaceHolder(), m_Allocator->MemAlloc(m_Allocator->Alloc, sizeof(Node))) Node{key};
		newNode->Next = newNode->Prev = nullptr;

		if (m_Buckets.Empty())
		{
			m_Buckets.Resize(9);
		}

		HashNodeInsert(newNode, Hash<Key>::Value(key), m_Buckets.Data(), m_Buckets.Size() - 1);

		++m_Size;

		ReHash();

		result.First.Node = newNode;
		result.Second     = true;
		return result;
	}

	template<typename Key>
	Pair<typename HashSet<Key>::Iterator, bool> HashSet<Key>::Emplace(const Key& key)
	{
		return Insert(key);
	}

	template<typename Key>
	template<typename ParamKey>
	bool HashSet<Key>::Has(const ParamKey& key) const
	{
		if (m_Buckets.Empty()) return false;
		const usize bucket = Hash<Key>::Value(key) & (m_Buckets.Size() - 2);
		typedef Node* NodePtr;
		for (NodePtr it = m_Buckets[bucket], end = m_Buckets[bucket + 1]; it != end; it = it->Next)
		{
			if (it->First == key)
			{
				return true;
			}
		}
		return false;
	}

	template<typename Key>
	HashSet<Key>::~HashSet()
	{
		Clear();
	}

	template<typename Key>
	void HashSet<Key>::ReHash()
	{
		if (m_Size + 1 > 4 * m_Buckets.Size())
		{
			Node* root = *m_Buckets.begin();
			const usize newNumberBuckets = (m_Buckets.Size() - 1) * 8;

			m_Buckets.Clear();
			m_Buckets.Resize(newNumberBuckets + 1);

			while (root)
			{
				Node*  next = root->Next;
				root->Next = root->Prev = 0;
				HashNodeInsert(root, Hash<Key>::Value(root->First), m_Buckets.Data(), m_Buckets.Size() - 1);
				root = next;
			}
		}
	}
}