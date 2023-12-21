// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.


#include "doctest.h"
#include "Skore/Core/Path.hpp"
#include "Skore/Core/Logger.hpp"

namespace Skore::Tests
{

	TEST_CASE("Core::PathTestBasics")
	{
		Path path{};
		path += "aa";
		path += "bb";
		path += "cc";
		path.Append(".asd");

		CHECK(path.Extension() == ".asd");
		CHECK(path.Name() == "cc");
		CHECK(path.Parent().Name() == "bb");



	}
}