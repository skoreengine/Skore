// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "doctest.h"
#include "Skore/Core/HashMap.hpp"

namespace Skore::Tests
{
	TEST_CASE("Core::HashMapTestBasics")
	{
		HashMap<int, int> map{};

		for (int i = 0; i < 1000; ++i)
		{
			map.Insert(MakePair(i,i * 100));
		}

		bool check = true;

		for (int i = 0; i < 1000; ++i)
		{
			HashMap<int, int>::Iterator it = map.Find(i);
			if (it == map.end())
			{
				REQUIRE(false);
			}

			if (it->second != i * 100)
			{
				check = false;
			}
		}

		CHECK(check);

		map.Clear();

		HashMap<int, int>::Iterator it = map.Find(1);
		REQUIRE(it == map.end());

	}

}