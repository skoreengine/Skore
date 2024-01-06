// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include <thread>
#include <iostream>
#include "doctest.h"
#include "Skore/Repository/Repository.hpp"
#include "Skore/Core/Reflection.hpp"
#include "Skore/EntryPoint.hpp"
#include <thread>

namespace Skore::Tests
{

	enum TestResource_
	{
		TestResource_BoolValue   = 0,
		TestResource_IntValue    = 1,
		TestResource_FloatValue  = 2,
		TestResource_StringValue = 3,
		TestResource_LongValue   = 4,
		TestResource_Subobject   = 5
	};

	enum TestOtherResource_
	{
		TestOtherResource_TestValue    = 0,
	};

	constexpr TypeID TestResource = GetTypeID<TestResource_>();
	constexpr TypeID TestOtherResource = GetTypeID<TestOtherResource_>();

	void CreateResourceTypes()
	{
		{
			ResourceFieldCreation fields[] = {
				ResourceFieldCreation{
					.Index = TestResource_BoolValue,
					.Name = "BoolValue",
					.Type = ResourceFieldType_Value,
					.FieldTypeId = GetTypeID<bool>()
				},
				ResourceFieldCreation{
					.Index = TestResource_IntValue,
					.Name = "IntValue",
					.Type = ResourceFieldType_Value,
					.FieldTypeId = GetTypeID<i32>()
				},
				ResourceFieldCreation{
					.Index = TestResource_FloatValue,
					.Name = "FloatValue",
					.Type = ResourceFieldType_Value,
					.FieldTypeId = GetTypeID<f32>()
				},
				ResourceFieldCreation{
					.Index = TestResource_StringValue,
					.Name = "StringValue",
					.Type = ResourceFieldType_Value,
					.FieldTypeId = GetTypeID<String>()
				},
				ResourceFieldCreation{
					.Index = TestResource_LongValue,
					.Name = "LongValue",
					.Type = ResourceFieldType_Value,
					.FieldTypeId = GetTypeID<i64>()
				},
				ResourceFieldCreation{
					.Index = TestResource_Subobject,
					.Name = "SubObject",
					.Type = ResourceFieldType_SubObject
				}
			};

			ResourceTypeCreation resourceTypeCreation{
				.Name = "TestResource",
				.TypeId = TestResource,
				.Fields = {fields, sizeof(fields) / sizeof(ResourceFieldCreation)}
			};
			Repository::CreateResourceType(resourceTypeCreation);
		}

		{
			ResourceFieldCreation fields[] = {
				ResourceFieldCreation{
					.Index = TestOtherResource_TestValue,
					.Name = "TestValue",
					.Type = ResourceFieldType_Value,
					.FieldTypeId = GetTypeID<i32>()
				}
			};

			ResourceTypeCreation resourceTypeCreation{
				.Name = "TestOtherResource",
				.TypeId = TestOtherResource,
				.Fields = {fields, sizeof(fields) / sizeof(ResourceFieldCreation)}
			};
			Repository::CreateResourceType(resourceTypeCreation);
		}
	}

	TEST_CASE("Repository::Basics")
	{
		Reflection::Init();
		Repository::Init();
		CreateResourceTypes();

		{
			RID rid = Repository::CreateResource(TestResource);

			{
				ResourceObject write = Repository::Write(rid);
				write.SetValue("IntValue", 102);
				write.SetValue("StringValue", String{"blahblah"});
				write.Commit();
			}

			ResourceObject originalRead = Repository::Read(rid);

			{
				CHECK(originalRead.GetValue<i32>("IntValue") == 102);
				CHECK(originalRead.GetValue<String>("StringValue") == "blahblah");
			}

			{
				ResourceObject write = Repository::Write(rid);
				write.SetValue("IntValue", 300);
			}

			{
				//no commit it will keep the original value
				ResourceObject read = Repository::Read(rid);
				CHECK(read.GetValue<i32>("IntValue") == 102);
			}

			{
				ResourceObject write = Repository::Write(rid);
				write.SetValue("IntValue", 300);
				write.Commit();
			}

			{
				ResourceObject read = Repository::Read(rid);
				CHECK(read.GetValue<i32>("IntValue") == 300);
			}

			//original read should be alive because it's not garbage collected yet.
			CHECK(originalRead.GetValue<i32>("IntValue") == 102);

			usize nr = GetAllocationNum();
			Repository::GarbageCollect();
			CHECK(nr > GetAllocationNum());

			{
				Repository::ClearValues(rid);
				ResourceObject read = Repository::Read(rid);
				CHECK(!read.Has("IntValue"));
				CHECK(!read.Has("StringValue"));
			}
		}

		Repository::Shutdown();
		Reflection::Shutdown();
	}

	TEST_CASE("Repository::Prototypes")
	{
		Reflection::Init();
		Repository::Init();

		CreateResourceTypes();

		//Create prototype
		RID prototype = Repository::CreateResource(TestResource);
		{
			ResourceObject write = Repository::Write(prototype);
			write.SetValue("IntValue", 300);
			write.SetValue("StringValue", String{"blahblah"});
			write.SetValue("FloatValue", 1.2f);
			write.SetValue("BoolValue", true);
			write.Commit();
		}

		RID rid = Repository::CreateFromPrototype(prototype);
		{
			//check original values
			ResourceObject read = Repository::Read(rid);
			CHECK(read.GetValue<i32>("IntValue") == 300);
			CHECK(read.GetValue<String>("StringValue") == "blahblah");
			CHECK(read.GetValue<f32>("FloatValue") == 1.2f);
			CHECK(read.GetValue<bool>("BoolValue") == true);
		}
		{
			//modify a value
			ResourceObject write = Repository::Write(rid);
			write.SetValue("StringValue", String{"another string"});
			write.SetValue("BoolValue", false);
			write.Commit();
		}

		{
			//check modified values
			ResourceObject read = Repository::Read(rid);
			CHECK(read.GetValue<i32>("IntValue") == 300);
			CHECK(read.GetValue<String>("StringValue") == "another string");
			CHECK(read.GetValue<f32>("FloatValue") == 1.2f);
			CHECK(read.GetValue<bool>("BoolValue") == false);
		}

		{
			//check if prototype has the same values
			ResourceObject read = Repository::Read(prototype);
			CHECK(read.GetValue<i32>("IntValue") == 300);
			CHECK(read.GetValue<String>("StringValue") == "blahblah");
			CHECK(read.GetValue<f32>("FloatValue") == 1.2f);
			CHECK(read.GetValue<bool>("BoolValue") == true);
		}

		{
			//change prototype
			ResourceObject write = Repository::Write(prototype);
			write.SetValue("IntValue", 500);
			write.SetValue("StringValue", String{"Prototype Changes"});
			write.Commit();
		}

		{
			//read it again modified values
			ResourceObject read = Repository::Read(rid);
			CHECK(read.GetValue<i32>("IntValue") == 500); //that's the prototype changed value.
			CHECK(read.GetValue<String>("StringValue") == "another string"); //this should keep the changed value
			CHECK(read.GetValue<f32>("FloatValue") == 1.2f); //original prototype value
			CHECK(read.GetValue<bool>("BoolValue") == false); //this should keep the changed value
		}

		{
			Repository::ClearValues(rid);

			//check original values again, it should be equals as the prototype.
			ResourceObject read = Repository::Read(rid);
			CHECK(read.GetValue<i32>("IntValue") == 500);
			CHECK(read.GetValue<String>("StringValue") == "Prototype Changes");
			CHECK(read.GetValue<f32>("FloatValue") == 1.2f);
			CHECK(read.GetValue<bool>("BoolValue") == true);
		}


		Repository::Shutdown();
		Reflection::Shutdown();
	}

	TEST_CASE("Repository::TestSubobjects")
	{
		Reflection::Init();
		Repository::Init();

		CreateResourceTypes();

		RID rid = Repository::CreateResource(TestResource);

		RID subobject = Repository::CreateResource(TestOtherResource);
		{
			ResourceObject write = Repository::Write(subobject);
			write.SetValue(TestOtherResource_TestValue, 10);
			write.Commit();
		}

		{
			ResourceObject write = Repository::Write(rid);
			write.SetSubobject(TestResource_Subobject, rid);
			write.Commit();
		}

		Repository::Shutdown();
		Reflection::Shutdown();
	}

	TEST_CASE("Repository::TestMultithreading")
	{
		usize tries = std::thread::hardware_concurrency() * 30;

		Allocator* bkpDefault = GetDefaultAllocator();

		Allocator allocator{
			.Alloc = nullptr,
			.MemAlloc = [](CPtr alloc, usize bytes)
			{
				return malloc(bytes);
			},
			.MemFree = [](CPtr alloc, CPtr ptr, usize bytes)
			{
				free(ptr);
			}
		};

		SetDefaultAllocator(allocator);

		Reflection::Init();
		Repository::Init();
		{
			CreateResourceTypes();

			u32 threads = std::max(std::thread::hardware_concurrency(), 4u);

			Array<std::thread> createThreads(threads);
			for (int i = 0; i < threads; ++i)
			{
				createThreads[i] = std::thread([&]()
				{
					for (int j = 0; j < tries; ++j)
					{
						Repository::CreateResource(TestResource);
					}
				});
			}

			for (int i = 0; i < threads; ++i)
			{
				createThreads[i].join();
			}

			//to write
			RID rid = Repository::CreateResource(TestResource);

			Array<std::thread> readThreads(threads);
			Array<std::thread> writeThreads(threads);
			std::atomic_bool check = false;

			for (int i = 0; i < threads; ++i)
			{
				writeThreads[i] = std::thread([&]()
				{
					for (int j = 0; j < tries; ++j)
					{
						ResourceObject object = Repository::Write(rid);
						object.SetValue(TestResource_LongValue, (i64) j);
						object.Commit();
					}
				});

				readThreads[i] = std::thread([&]()
				{
					for (int j = 0; j < tries * 10; ++j)
					{
						ResourceObject object = Repository::Read(rid);
						if (object)
						{
							if (object.GetValue<i64>(TestResource_LongValue) >= 0)
							{
								check = true;
							}
						}
					}
				});
			}

			CHECK(check);

			for (int i = 0; i < threads; ++i)
			{
				readThreads[i].join();
				writeThreads[i].join();
			}
		}

		Repository::Shutdown();
		Reflection::Shutdown();

		SetDefaultAllocator(*bkpDefault);
	}
}