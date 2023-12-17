// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "doctest.h"
#include "Skore/Core/String.hpp"
#include "Skore/Core/Array.hpp"


namespace Skore
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

		arrInt.Clear();
		CHECK(arrInt.Empty());
		CHECK(arrInt.Size() == 0);
		CHECK(arrInt.Capacity() == 10);
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
}

