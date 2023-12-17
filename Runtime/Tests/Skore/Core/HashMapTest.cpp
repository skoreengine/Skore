// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "doctest.h"
#include "Skore/Core/HashMap.hpp"

namespace Skore::Tests
{
	TEST_CASE("Core::HashMapTestBasics")
	{
		HashMap<int, int> map{};
	}
}