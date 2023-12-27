// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Skore/Core/TypeInfo.hpp"
#include "doctest.h"
#include "Skore/Core/String.hpp"

namespace Skore::Tests
{
	struct TypeIDTest
	{
		i32 A{};
	};

	TEST_CASE("Core::TypeIDBasics")
	{
		String typeName = GetTypeName<TypeIDTest>();
		TypeID typeId = GetTypeID<TypeIDTest>();
        CHECK(typeId > 0);
		CHECK(typeName == "Skore::Tests::TypeIDTest");

		TypeInfo typeInfo = GetTypeInfo<TypeIDTest>();
		CHECK(typeInfo.Size > 0);
	}
}
