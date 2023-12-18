// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Skore/Core/TypeID.hpp"
#include "doctest.h"
#include "Skore/Core/String.hpp"

namespace Skore::Tests
{
	struct TypeIDTest{};

	TEST_CASE("Core::TypeIDBasics")
	{
		String typeName = GetTypeName<TypeIDTest>();
		TypeID typeId = GetTypeID<TypeIDTest>();
		CHECK(typeName == "Skore::Tests::TypeIDTest");
		CHECK(10386068229281325449ull == typeId); //TODO : check this on other platforms
	}
}
