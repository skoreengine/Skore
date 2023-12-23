// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "doctest.h"
#include "Skore/Core/HashMap.hpp"
#include "Skore/Core/String.hpp"
#include "Skore/Core/StringView.hpp"
#include <string>

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

		CHECK(map.Has(100));

		for (int i = 0; i < 1000; ++i)
		{
			HashMap<int, int>::Iterator it = map.Find(i);
			if (it == map.end())
			{
				REQUIRE(false);
			}

			if (it->Second != i * 100)
			{
				check = false;
			}
		}

		CHECK(check);

		map.Clear();

		HashMap<int, int>::Iterator it = map.Find(1);
		REQUIRE(it == map.end());

	}

	TEST_CASE("Core::HashMapTestForeach")
	{
		HashMap<int, int> map{};
		map.Insert(MakePair(1, 20));
		map.Insert(MakePair(2, 40));

		i32 sum = 0;
		for(HashMap<int, int>::Node& it : map)
		{
			sum += it.Second;
		}

		CHECK(sum == 60);
	}

	TEST_CASE("Core::HashMapTestMove")
	{
		HashMap<int, int> map{};
		map.Insert(MakePair(1, 20));
		map.Insert(MakePair(2, 40));

		HashMap<int, int> other{Traits::Move(map)};
		CHECK(other[2] == 40);
		CHECK(map.Empty());
	}

	TEST_CASE("Core::HashMapTestCopy")
	{
		HashMap<int, int> map{};
		map.Insert(MakePair(1, 20));
		map.Insert(MakePair(2, 40));

		HashMap<int, int> other = map;

		CHECK(map[1] == 20);
		CHECK(map[2] == 40);

		CHECK(other[1] == 20);
		CHECK(other[2] == 40);

		CHECK(other.Size() == 2);
	}

	TEST_CASE("Core::HashMapTestErase")
	{
		HashMap<int, int> map{};
		map.Insert(MakePair(1, 20));
		map.Insert(MakePair(2, 40));

		map.Erase(map.Find(1));

		CHECK(map.Find(1) == map.end());
		CHECK(map.Find(2) != map.end());

		map.Erase(2);

		CHECK(map.Find(2) == map.end());
	}

	TEST_CASE("Core::HashMapTestStr")
	{
		HashMap<String, String> map{};
		map["AAAA"] = "BBBB";
		map["CCCC"] = "DDDD";

		for (int i = 0; i < 10000; ++i)
		{
			std::string str = std::to_string(i);
			map.Insert(String{str.c_str()}, String{str.c_str()});
		}

		{
			auto it = map.Find("CCCC");
			REQUIRE(it != map.end());
			CHECK(it->Second == "DDDD");
		}

		{
			StringView strView = {"AAAA"};
			auto it = map.Find(strView);
			REQUIRE(it != map.end());
			CHECK(it->Second == "BBBB");
		}
	}
	TEST_CASE("Core::HashMapTestEmplace")
	{
		HashMap<String, String> map{};
		map.Emplace("AAA", "BBB");
		CHECK(map.Has("AAA"));
	}


}