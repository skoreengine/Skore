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


		usize pos = stringView.FindFirstOf("c");
		CHECK(pos == 2);

	}
}
