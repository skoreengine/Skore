// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.


#include "doctest.h"
#include "Skore/Core/StringView.hpp"

namespace Skore::Tests
{
	TEST_CASE("Core::StringViewBasis")
	{
		StringView stringView = {"abcdce"};
		CHECK(!stringView.Empty());

		CHECK(stringView.FindFirstOf("c") == 2);
		CHECK(stringView.FindFirstOf('c') == 2);
		CHECK(stringView.FindFirstNotOf("a") == 1);
		CHECK(stringView.FindLastOf("c") == 4);
		CHECK(stringView.FindLastNotOf("e") == 4);
	}
}
