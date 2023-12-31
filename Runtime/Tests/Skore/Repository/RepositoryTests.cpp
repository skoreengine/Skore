// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "doctest.h"
#include "Skore/Repository/Repository.hpp"
#include "Skore/Core/Reflection.hpp"


namespace Skore::Tests
{
	TEST_CASE("Repository::Basics")
	{
		Repository::Init();

		ResourceFieldCreation fields[] = {
			ResourceFieldCreation{
				.Name = "BoolValue",
				.Type = ResourceFieldType_Bool
			},
			ResourceFieldCreation{
				.Name = "IntValue",
				.Type = ResourceFieldType_Int
			},
			ResourceFieldCreation{
				.Name = "FloatValue",
				.Type = ResourceFieldType_Float
			},
			ResourceFieldCreation{
				.Name = "StringValue",
				.Type = ResourceFieldType_String
			}
		};

		ResourceTypeCreation resourceTypeCreation{
			.Name = "TestResource",
			.TypeId = HashValue("TestResource"),
			.Fields = fields
		};

		Repository::CreateResourceType(resourceTypeCreation);

		RID rid = Repository::CreateResource(HashValue("TestResource"));
		ResourceSet* write = Repository::Write(rid);
		write->SetInt("IntValue", 102);
		write->Commit();

		const ResourceSet* read = Repository::Read(rid);
		//CHECK(read->GetInt("IntValue") == 102);


		Reflection::Shutdown();
		Repository::Shutdown();
	}
}