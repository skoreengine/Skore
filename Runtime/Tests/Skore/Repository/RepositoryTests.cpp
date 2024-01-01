// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "doctest.h"
#include "Skore/Repository/Repository.hpp"
#include "Skore/Core/Reflection.hpp"


namespace Skore::Tests
{
	TEST_CASE("Repository::Basics")
	{
		Reflection::Init();
		Repository::Init();
		{
			ResourceFieldCreation fields[] = {
				ResourceFieldCreation{
					.Name = "BoolValue",
					.Type = GetTypeID<bool>()
				},
				ResourceFieldCreation{
					.Name = "IntValue",
					.Type = GetTypeID<i32>()
				},
				ResourceFieldCreation{
					.Name = "FloatValue",
					.Type = GetTypeID<f32>()
				},
				ResourceFieldCreation{
					.Name = "StringValue",
					.Type = GetTypeID<String>()
				}
			};

			ResourceTypeCreation resourceTypeCreation{
				.Name = "TestResource",
				.TypeId = HashValue("TestResource"),
				.Fields = {fields, sizeof(fields) / sizeof(ResourceFieldCreation)}
			};

			Repository::CreateResourceType(resourceTypeCreation);

			RID rid = Repository::CreateResource(HashValue("TestResource"));

			{
				ResourceObject write = Repository::Write(rid);
				write.Set("IntValue", 102);
				write.Set("StringValue", String{"blahblah"});
				write.Commit();
			}

			ResourceObject originalRead = Repository::Read(rid);

			{
				CHECK(originalRead.Get<i32>("IntValue") == 102);
				CHECK(originalRead.Get<String>("StringValue") == "blahblah");
			}

			{
				ResourceObject write = Repository::Write(rid);
				write.Set("IntValue", 300);
			}

			{
				//no commit it will keep the original value
				ResourceObject read = Repository::Read(rid);
				CHECK(read.Get<i32>("IntValue") == 102);
			}

			{
				ResourceObject write = Repository::Write(rid);
				write.Set("IntValue", 300);
				write.Commit();
			}

			{
				ResourceObject read = Repository::Read(rid);
				CHECK(read.Get<i32>("IntValue") == 300);
			}

			//original read should be alive because it's not garbage collected yet.
			//CHECK(originalRead.Get<i32>("IntValue") == 102);
		}

		Repository::Shutdown();
		Reflection::Shutdown();
	}
}