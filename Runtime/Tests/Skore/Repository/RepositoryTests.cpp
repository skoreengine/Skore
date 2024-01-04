// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include <thread>
#include <iostream>
#include "doctest.h"
#include "Skore/Repository/Repository.hpp"
#include "Skore/Core/Reflection.hpp"
#include "Skore/Platform/Platform.hpp"
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
	};

	constexpr TypeID TestResource = GetTypeID<TestResource_>();

	void CreateTestResource()
	{
		ResourceFieldCreation fields[] = {
			ResourceFieldCreation{
				.Index = TestResource_BoolValue,
				.Name = "BoolValue",
				.Type = GetTypeID<bool>()
			},
			ResourceFieldCreation{
				.Index = TestResource_IntValue,
				.Name = "IntValue",
				.Type = GetTypeID<i32>()
			},
			ResourceFieldCreation{
				.Index = TestResource_FloatValue,
				.Name = "FloatValue",
				.Type = GetTypeID<f32>()
			},
			ResourceFieldCreation{
				.Index = TestResource_StringValue,
				.Name = "StringValue",
				.Type = GetTypeID<String>()
			},
			ResourceFieldCreation{
				.Index = TestResource_LongValue,
				.Name = "LongValue",
				.Type = GetTypeID<i64>()
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

			//original read should be alive because it's not garbage collected yet.
			CHECK(originalRead.Get<i32>("IntValue") == 102);

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
			ResourceObject read = Repository::Read(rid);
			CHECK(read.Get<i32>("IntValue") == 500); //that's the prototype changed value.
			CHECK(read.Get<String>("StringValue") == "another string"); //this should keep the changed value
			CHECK(read.Get<f32>("FloatValue") == 1.2f); //original prototype value
			CHECK(read.Get<bool>("BoolValue") == false); //this should keep the changed value
		}

		{
			Repository::ClearValues(rid);

			//check original values again, it should be equals as the prototype.
			ResourceObject read = Repository::Read(rid);
			CHECK(read.Get<i32>("IntValue") == 500);
			CHECK(read.Get<String>("StringValue") == "Prototype Changes");
			CHECK(read.Get<f32>("FloatValue") == 1.2f);
			CHECK(read.Get<bool>("BoolValue") == true);
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
			CreateTestResource();

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
						object.Set(TestResource_LongValue, (i64) j);
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
							if (object.Get<i64>(TestResource_LongValue) >= 0)
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