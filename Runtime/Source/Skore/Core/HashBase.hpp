// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

namespace Skore
{
	template<typename Key, typename Value>
	struct HashNode
	{
		const Key First;
		Value     Second;

		HashNode* Next;
		HashNode* Prev;
	};

	template<typename Key>
	struct HashNode<Key, void>
	{
		const Key First;

		HashNode* Next;
		HashNode* Prev;
	};


	template<typename T>
	struct HashIterator
	{
		T* Node{};
		T* operator->() const;
		T& operator*() const;
		explicit operator bool() const noexcept;
	};


	template<typename T>
	struct HashIterator<const T>
	{
		HashIterator() = default;

		HashIterator(T* other) : Node(other)
		{
		}

		HashIterator(HashIterator<T> other) : Node(other.Node)
		{
		}

		T* operator->() const;
		T& operator*() const;
		explicit operator bool() const noexcept;
		T* Node{};
	};

	template<typename Node>
	inline Node* HashIterator<Node>::operator->() const
	{
		return Node;
	}

	template<typename Node>
	inline Node& HashIterator<Node>::operator*() const
	{
		return *Node;
	}
	template<typename Node>
	inline Node* HashIterator<const Node>::operator->() const
	{
		return Node;
	}

	template<typename Node>
	inline Node& HashIterator<const Node>::operator*() const
	{
		return *Node;
	}

	template<typename Node>
	static inline void operator++(HashIterator<Node>& lhs)
	{
		lhs.Node = lhs.Node->Next;
	}

	template<typename LNode, typename RNode>
	static inline bool operator==(const HashIterator<LNode>& lhs, const HashIterator<RNode>& rhs)
	{
		return lhs.Node == rhs.Node;
	}

	template<typename LNode, typename RNode>
	static inline bool operator!=(const HashIterator<LNode>& lhs, const HashIterator<RNode>& rhs)
	{
		return lhs.Node != rhs.Node;
	}

	template<typename Node>
	HashIterator<Node>::operator bool() const noexcept
	{
		return Node != nullptr;
	}

	template<typename Node>
	HashIterator<const Node>::operator bool() const noexcept
	{
		return Node != nullptr;
	}

	template<typename Key, typename Value>
	static inline void HashNodeErase(const HashNode<Key, Value>* where, usize hash, HashNode<Key, Value>** buckets, usize sizeBuckets)
	{
		usize bucket = hash & (sizeBuckets - 1);
		HashNode<Key, Value>* next = where->Next;
		for (; buckets[bucket] == where; --bucket)
		{
			buckets[bucket] = next;
			if (!bucket)
			{
				break;
			}
		}

		if (where->Prev)
		{
			where->Prev->Next = where->Next;
		}

		if (next)
		{
			next->Prev = where->Prev;
		}
	}


	template<typename Key, typename Value>
	static inline void HashNodeInsert(HashNode<Key, Value>* node, usize hash, HashNode<Key, Value>** buckets, usize sizeBuckets)
	{
		usize bucket = hash & (sizeBuckets - 1);
		HashNode<Key, Value>* it = buckets[bucket + 1];
		node->Next = it;
		if (it)
		{
			node->Prev           = it->Prev;
			it->Prev             = node;
			if (node->Prev)
				node->Prev->Next = node;
		}
		else
		{
			usize newBucket = bucket;
			while (newBucket && !buckets[newBucket])
			{
				--newBucket;
			}

			HashNode<Key, Value>* prev = buckets[newBucket];
			while (prev && prev->Next)
			{
				prev = prev->Next;
			}

			node->Prev = prev;
			if (prev)
			{
				prev->Next = node;
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