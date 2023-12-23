// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "doctest.h"
#include "Skore/Core/UUID.hpp"
#include "Skore/Core/String.hpp"

namespace Skore::Tests
{
	TEST_CASE("Core::UUIDBasics")
	{
		{
			UUID uuid{};
			CHECK(!uuid);
		}

		{
			UUID uuid{1000, 2000};
			CHECK(uuid);
		}

		{
			UUID random = UUID::RandomUUID();
			CHECK(random);
		}
		{
			UUID fromString = UUID::FromString("96ba841e-24f5-4622-8c09-bb7385580abb");
			CHECK(fromString.FirstValue > 0);
			CHECK(fromString.SecondValue > 0);

			CHECK(HashValue(fromString) > 0);

			String string{};
			string.Append(fromString);
			CHECK("96ba841e-24f5-4622-8c09-bb7385580abb" == string);
		}

		{
			auto   uuid = UUID{};
			String str{};
			str.Append(uuid);
			CHECK(str == "00000000-0000-0000-0000-000000000000");
			CHECK(UUID::FromString(str) == uuid);
		}

	}
}