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

}