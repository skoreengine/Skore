// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "doctest.h"
#include "Skore/Core/String.hpp"
#include "Skore/Core/Array.hpp"


namespace Skore::Tests
{

	TEST_CASE("Core::ArrayTestBasics")
	{
		Array<i32> arrInt{};
		CHECK(arrInt.Empty());

		arrInt.Reserve(10);
		arrInt.EmplaceBack(1);
		arrInt.EmplaceBack(2);
		arrInt.EmplaceBack(3);

		CHECK(!arrInt.Empty());
		CHECK(arrInt.Size() == 3);
		CHECK(arrInt.Capacity() == 10);

		CHECK(arrInt.Data() != nullptr);

		CHECK(arrInt[0] == 1);
		CHECK(arrInt[1] == 2);
		CHECK(arrInt[2] == 3);

		i32 i = 0;
		for(const auto it: arrInt)
		{
			CHECK(it == ++i);
		}

		arrInt.PopBack();
		CHECK(arrInt.Size() == 2);

		arrInt.Clear();
		CHECK(arrInt.Empty());
		CHECK(arrInt.Size() == 0);
		CHECK(arrInt.Capacity() == 10);
	}

	TEST_CASE("Core::ArrayTestResize")
	{
		Array<i32> arrInt{};
		arrInt.Resize(10, 5);
		for (int i = 0; i < 10; ++i)
		{
			CHECK(arrInt[i] == 5);
		}
	}

	TEST_CASE("Core::ArrayTestInsertBegin")
	{
		Array<i32> arrInt{};
		arrInt.EmplaceBack(1);
		arrInt.EmplaceBack(2);
		arrInt.EmplaceBack(3);


		{
			Array<i32> arrOther{};
			arrOther.EmplaceBack(10);
			arrOther.EmplaceBack(20);
			arrInt.Insert(arrInt.begin(), arrOther.begin(), arrOther.end());
		}

		REQUIRE(arrInt.Size() == 5);

		CHECK(arrInt[0] == 10);
		CHECK(arrInt[1] == 20);
		CHECK(arrInt[2] == 1);
		CHECK(arrInt[3] == 2);
		CHECK(arrInt[4] == 3);
	}

	TEST_CASE("Core::ArrayTestInsertMiddle")
	{
		Array<i32> arrInt{};
		arrInt.EmplaceBack(1);
		arrInt.EmplaceBack(2);
		arrInt.EmplaceBack(3);


		{
			Array<i32> arrOther{};
			arrOther.EmplaceBack(10);
			arrOther.EmplaceBack(20);
			arrInt.Insert(arrInt.begin() + 2, arrOther.begin(), arrOther.end());
		}

		REQUIRE(arrInt.Size() == 5);

		CHECK(arrInt[0] == 1);
		CHECK(arrInt[1] == 2);
		CHECK(arrInt[2] == 10);
		CHECK(arrInt[3] == 20);
		CHECK(arrInt[4] == 3);
	}

	TEST_CASE("Core::ArrayTestInsertEnd")
	{
		Array<i32> arrInt{};
		arrInt.EmplaceBack(1);
		arrInt.EmplaceBack(2);
		arrInt.EmplaceBack(3);

		{
			Array<i32> arrOther{};
			arrOther.EmplaceBack(10);
			arrOther.EmplaceBack(20);
			arrInt.Insert(arrInt.end(), arrOther.begin(), arrOther.end());
		}

		REQUIRE(arrInt.Size() == 5);

		CHECK(arrInt[0] == 1);
		CHECK(arrInt[1] == 2);
		CHECK(arrInt[2] == 3);
		CHECK(arrInt[3] == 10);
		CHECK(arrInt[4] == 20);
	}

	TEST_CASE("Core::ArrayTestErase")
	{
		Array<i32> arrInt{};
		arrInt.EmplaceBack(1);

		arrInt.EmplaceBack(2);
		arrInt.EmplaceBack(3);
		arrInt.EmplaceBack(4);

		arrInt.EmplaceBack(5);
		arrInt.EmplaceBack(6);

		arrInt.Erase(arrInt.begin() + 1, arrInt.begin() + 4);

		CHECK(arrInt.Size() == 3);

		CHECK(arrInt[0] == 1);
		CHECK(arrInt[1] == 5);
		CHECK(arrInt[2] == 6);
	}

	TEST_CASE("Core::ArrayTestCopy")
	{
		Array<i32> arrInt{};
		arrInt.EmplaceBack(1);
		arrInt.EmplaceBack(2);
		arrInt.EmplaceBack(3);

		Array<i32> copy{arrInt};

		CHECK(copy[0] == 1);
		CHECK(copy[1] == 2);
		CHECK(copy[2] == 3);

		Array<i32> assign = arrInt;
		CHECK(assign[0] == 1);
		CHECK(assign[1] == 2);
		CHECK(assign[2] == 3);
	}

	TEST_CASE("Core::ArrayTestMove")
	{
		Array<i32> arrInt{};
		arrInt.EmplaceBack(1);
		arrInt.EmplaceBack(2);
		arrInt.EmplaceBack(3);

		Array<i32> move = Traits::Move(arrInt);

		CHECK(move[0] == 1);
		CHECK(move[1] == 2);
		CHECK(move[2] == 3);

		CHECK(arrInt.Empty());
	}

	Array<i32> GetArray()
	{
		Array<i32> arrInt{};
		arrInt.EmplaceBack(1);
		arrInt.EmplaceBack(2);
		arrInt.EmplaceBack(3);
		return arrInt;
	}

	TEST_CASE("Core::ArrayTestMoveFunc")
	{
		Array<i32> arr = GetArray();
		CHECK(arr[0] == 1);
		CHECK(arr[1] == 2);
		CHECK(arr[2] == 3);
		CHECK(!arr.Empty());
	}
}