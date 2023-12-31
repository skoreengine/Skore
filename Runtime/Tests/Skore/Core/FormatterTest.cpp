// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Skore/Core/Formatter.hpp"
#include "doctest.h"

namespace Skore::Tests
{

	TEST_CASE("Core::FormatterTestBasics")
	{
		{
			char buffer[100]{};
			Formatter::FormatTo("test {} {} {}", buffer, 20, 30, 50);
			CHECK(StringView(buffer) == "test 20 30 50");
		}

		{
			char buffer[100]{};
			Formatter::FormatTo("test {}", buffer, "str");
			CHECK(StringView(buffer) == "test str");
		}
	}
}