// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Skore/Core/Algorithm.hpp"
#include "doctest.h"
#include "Skore/Core/String.hpp"
#include "Skore/Core/StringView.hpp"

namespace Skore::Tests
{

	TEST_CASE("Core::Algorithm::Split")
	{
		StringView str = "Hello World";
		StringView delimiter = " ";

		String result;
		Split(str, delimiter, [&result](const String& split)
		{
			result += split;
		});

		CHECK(result == "HelloWorld");
	}



	TEST_CASE("Core::Algorithm::Sort")
	{
		i32 arr[20] = {2, 3, 2, 0, 5, 1, 8, 6, 4, 7, 10, 9, 15, 11, 13, 12, 14, 17, 16, 20};

		Sort(arr, arr + 20, [](i32 l, i32 r)
		{
			return l < r;
		});

		CHECK(arr[0] == 0);
		CHECK(arr[1] == 1);
		CHECK(arr[2] == 2);
		CHECK(arr[3] == 2);
		CHECK(arr[4] == 3);
		CHECK(arr[5] == 4);
		CHECK(arr[6] == 5);
		CHECK(arr[7] == 6);
		CHECK(arr[8] == 7);
		CHECK(arr[9] == 8);
		CHECK(arr[10] == 9);
		CHECK(arr[11] == 10);
		CHECK(arr[12] == 11);
		CHECK(arr[13] == 12);
		CHECK(arr[14] == 13);
		CHECK(arr[15] == 14);
		CHECK(arr[16] == 15);
		CHECK(arr[17] == 16);
		CHECK(arr[18] == 17);
		CHECK(arr[19] == 20);
	}

}