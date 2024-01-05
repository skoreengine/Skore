// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.


#include "Skore/Defines.hpp"
#include "Skore/Core/Algorithm.hpp"
#include "doctest.h"

namespace Skore::Tests
{

	TEST_CASE("Core::Algorithm::Sort")
	{
		i32 arr[4] = {2, 3, 2, 0};

		Sort(arr, arr + 4, [](i32 l, i32 r)
		{
			return l > r;
		});

		CHECK(arr[0] == 0);
		CHECK(arr[1] == 2);
		CHECK(arr[2] == 2);
		CHECK(arr[3] == 3);
	}
}

