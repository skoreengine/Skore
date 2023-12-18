// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Skore/Core/Function.hpp"
#include "doctest.h"

namespace Skore::Tests
{
	TEST_CASE("Core::FunctionBasics")
	{
		{
			Function<void()> nullFunc = {};
			CHECK(!nullFunc);
		}


		{
			bool check = false;

			Function<void()> funcNoParamsNoRet = [&]()
			{
				check = true;
			};

			CHECK(funcNoParamsNoRet);

			funcNoParamsNoRet();
			CHECK(check);
		}

		{
			Function<i32(i32, i32)> funcParamsRet = [](i32 a, i32 b)
			{
				return a + b;
			};
			CHECK(funcParamsRet(10, 20) == 30);
		}

		{
			Function<i32()> funcNoParamsRet = []()
			{
				return 10;
			};
			;
			CHECK(funcNoParamsRet() == 10);
		}
	}

	TEST_CASE("Core::FunctionMoveCopy")
	{
		Function<i32(i32, i32)> funcParamsRet = [](i32 a, i32 b)
		{
			return a + b;
		};

		Function<i32(i32, i32)> funcParamsRetCopy = funcParamsRet;
		CHECK(funcParamsRetCopy(10, 20) == 30);

	}
}