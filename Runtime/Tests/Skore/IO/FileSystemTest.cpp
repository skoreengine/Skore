// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "doctest.h"
#include "Skore/IO/FileSystem.hpp"

namespace Skore::Tests
{
	TEST_CASE("IO::FIleSystemBasics")
	{
		CHECK(!FileSystem::CurrentDir().GetString().Empty());
		CHECK(!FileSystem::AppFolder().GetString().Empty());
		CHECK(!FileSystem::DocumentsDir().GetString().Empty());

		CHECK(FileSystem::CreateDirectory("test"));
		FileStatus status = FileSystem::FileStatus("test");
		CHECK(status.exists);
		CHECK(status.isDirectory);
		CHECK(status.lastModifiedTime > 0);
		CHECK(FileSystem::Remove("test"));
	}
};