// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "doctest.h"
#include "Skore/Repository/Repository.hpp"
#include "Skore/Core/Reflection.hpp"

namespace Skore::Tests
{
	constexpr TypeID TestResource = HashValue("TestResource");

	void CreateTestResource()
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
			.TypeId = TestResource,
			.Fields = {fields, sizeof(fields) / sizeof(ResourceFieldCreation)}
		};
		Repository::CreateResourceType(resourceTypeCreation);
	}

	TEST_CASE("Repository::Basics")
	{
		Reflection::Init();
		Repository::Init();
		CreateTestResource();

		{
			RID rid = Repository::CreateResource(TestResource);

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

//			original read should be alive because it's not garbage collected yet.
			//CHECK(originalRead.Get<i32>("IntValue") == 102);
		}

		Repository::Shutdown();
		Reflection::Shutdown();
	}

	TEST_CASE("Repository::Prototypes")
	{
		Reflection::Init();
		Repository::Init();

		CreateTestResource();

		//Create prototype
		RID prototype = Repository::CreateResource(TestResource);
		{
			ResourceObject write = Repository::Write(prototype);
			write.Set("IntValue", 300);
			write.Set("StringValue", String{"blahblah"});
			write.Set("FloatValue", 1.2f);
			write.Set("BoolValue", true);
			write.Commit();
		}

		RID rid = Repository::CreateFromPrototype(prototype);
		{
			//check original values
			ResourceObject read = Repository::Read(rid);
			CHECK(read.Get<i32>("IntValue") == 300);
			CHECK(read.Get<String>("StringValue") == "blahblah");
			CHECK(read.Get<f32>("FloatValue") == 1.2f);
			CHECK(read.Get<bool>("BoolValue") == true);
		}
		{
			//modify a value
			ResourceObject write = Repository::Write(rid);
			write.Set("StringValue", String{"another string"});
			write.Set("BoolValue", false);
			write.Commit();
		}

		{
			//check modified values
			ResourceObject read = Repository::Read(rid);
			CHECK(read.Get<i32>("IntValue") == 300);
			CHECK(read.Get<String>("StringValue") == "another string");
			CHECK(read.Get<f32>("FloatValue") == 1.2f);
			CHECK(read.Get<bool>("BoolValue") == false);
		}

		{
			//check if prototype has the same values
			ResourceObject read = Repository::Read(prototype);
			CHECK(read.Get<i32>("IntValue") == 300);
			CHECK(read.Get<String>("StringValue") == "blahblah");
			CHECK(read.Get<f32>("FloatValue") == 1.2f);
			CHECK(read.Get<bool>("BoolValue") == true);
		}

		{
			//change prototype
			ResourceObject write = Repository::Write(prototype);
			write.Set("IntValue", 500);
			write.Set("StringValue", String{"Prototype Changes"});
			write.Commit();
		}

		{
			//read it again modified values
			//TODO: Still not working yet.
//			ResourceObject read = Repository::Read(rid);
//			CHECK(read.Get<i32>("IntValue") == 500); //that's the prototype changed value.
//			CHECK(read.Get<String>("StringValue") == "another string"); //this should keep the changed value
//			CHECK(read.Get<f32>("FloatValue") == 1.2f); //original prototype value
//			CHECK(read.Get<bool>("BoolValue") == false); //this should keep the changed value
		}


		Repository::Shutdown();
		Reflection::Shutdown();
	}
}