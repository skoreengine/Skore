// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "doctest.h"

#include "Skore/Core/Reflection.hpp"

namespace Skore::Tests
{

	struct ReflectionTypeTest
	{
		u32 uint;
		i32 iint;
		String string;
	};


	TEST_CASE("Core::ReflectionBasics")
	{
		Reflection::Type<ReflectionTypeTest>();



		Reflection::Shutdown();
	}
}