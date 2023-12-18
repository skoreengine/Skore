// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.


#include "Skore/Core/TypeInfo.hpp"
#include "doctest.h"

#include <iostream>

namespace Skore::Tests
{
	struct TypeTest{};


	TEST_CASE("Core::TypeIDBasics")
	{
		TypeID typeId = GetTypeID<TypeTest>();
		std::cout << typeId << std::endl;

	}
}
