// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

namespace Skore
{

	template<typename Key, typename Value>
	struct HashNode
	{
		const Key first;
		Value     second;

		HashNode* next;
		HashNode* prev;
	};

	template<typename Node>
	struct HashIterator
	{
		Node* operator->() const;
		Node& operator*() const;
		Node* node{};
	};

	template<typename Node>
	inline Node* HashIterator<Node>::operator->() const
	{
		return node;
	}

	template<typename Node>
	inline Node& HashIterator<Node>::operator*() const
	{
		return *node;
	}

	template<typename LNode, typename RNode>
	static inline bool operator==(const HashIterator<LNode>& lhs, const HashIterator<RNode>& rhs)
	{
		return lhs.node == rhs.node;
	}

	template<typename LNode, typename RNode>
	static inline bool operator!=(const HashIterator<LNode>& lhs, const HashIterator<RNode>& rhs)
	{
		return lhs.node != rhs.node;
	}

	template<typename Node>
	static inline void operator++(HashIterator<Node>& lhs)
	{
		lhs.node = lhs.node->next;
	}


	template<typename Key, typename Value>
	static inline void HashNodeInsert(HashNode<Key, Value>* node, usize hash, HashNode<Key, Value>** buckets, usize sizeBuckets)
	{
		usize bucket = hash & (sizeBuckets - 1);
		HashNode<Key, Value>* it = buckets[bucket + 1];
		node->next = it;
		if (it)
		{
			node->prev           = it->prev;
			it->prev             = node;
			if (node->prev)
				node->prev->next = node;
		}
		else
		{
			usize newBucket = bucket;
			while (newBucket && !buckets[newBucket])
			{
				--newBucket;
			}

			HashNode<Key, Value>* prev = buckets[newBucket];
			while (prev && prev->next)
			{
				prev = prev->next;
			}

			node->prev = prev;
			if (prev)
			{
				prev->next = node;
			}
		}

		// propagate node through buckets
		for (; it == buckets[bucket]; --bucket)
		{
			buckets[bucket] = node;
			if (!bucket)
				break;
		}
	}

}