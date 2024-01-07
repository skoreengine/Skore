// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include <thread>
#include <iostream>
#include "doctest.h"
#include "Skore/Repository/Repository.hpp"
#include "Skore/Core/Reflection.hpp"
#include "Skore/EntryPoint.hpp"

namespace Skore::Tests
{

	enum TestResource_
	{
		TestResource_BoolValue    = 0,
		TestResource_IntValue     = 1,
		TestResource_FloatValue   = 2,
		TestResource_StringValue  = 3,
		TestResource_LongValue    = 4,
		TestResource_SubObject    = 5,
		TestResource_SubObjectSet = 6
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
					.Index = TestResource_SubObject,
					.Name = "SubObject",
					.Type = ResourceFieldType_SubObject
				},
				ResourceFieldCreation{
					.Index = TestResource_SubObjectSet,
					.Name = "SubObjectSet",
					.Type = ResourceFieldType_SubObjectSet
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
			UUID uuid = UUID::RandomUUID();
			Repository::SetUUID(rid, uuid);

			CHECK(Repository::GetUUID(rid) == uuid);
			CHECK(Repository::GetByUUID(uuid) == rid);

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

			CHECK(Repository::IsAlive(rid) == true);
			Repository::DestroyResource(rid);
			Repository::GarbageCollect();
			CHECK(Repository::IsAlive(rid) == false);
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

	TEST_CASE("Repository::TestSubObjects")
	{
		Reflection::Init();
		Repository::Init();

		CreateResourceTypes();
		{
			RID rid = Repository::CreateResource(TestResource);
			RID subobject = Repository::CreateResource(TestOtherResource);
			{
				ResourceObject write = Repository::Write(subobject);
				write.SetValue(TestOtherResource_TestValue, 10);
				write.Commit();
			}

			{
				ResourceObject write = Repository::Write(rid);
				write.SetSubObject(TestResource_SubObject, subobject);
				write.Commit();
			}

			{
				ResourceObject write = Repository::Write(rid);
				write.SetValue(TestResource_StringValue, String{"teststr"});
				write.Commit();
			}

			{
				ResourceObject read = Repository::Read(rid);
				CHECK(read.GetSubObject(TestResource_SubObject) == subobject);
			}

			Repository::DestroyResource(rid);
			Repository::GarbageCollect();
			CHECK(Repository::IsAlive(rid) == false);
			CHECK(Repository::IsAlive(subobject) == false);
		}

		Repository::Shutdown();
		Reflection::Shutdown();
	}

	TEST_CASE("Repository::TestSubObjectsSet")
	{
		Reflection::Init();
		Repository::Init();

		CreateResourceTypes();

		RID rid = Repository::CreateResource(TestResource);

		{
			ResourceObject write = Repository::Write(rid);
			for (int i = 0; i < 10; ++i)
			{
				RID subObject = Repository::CreateResource(TestOtherResource);

				ResourceObject writeSub = Repository::Write(subObject);
				writeSub.SetValue(TestOtherResource_TestValue, (i32) subObject.Id);
				writeSub.Commit();

				write.AddToSubObjectSet(TestResource_SubObjectSet, subObject);
			}
			write.Commit();
		}

		{
			ResourceObject read = Repository::Read(rid);

			Array<RID> subObjects{};
			subObjects.Resize(read.GetSubObjectSetCount(TestResource_SubObjectSet));
			read.GetSubObjectSet(TestResource_SubObjectSet, subObjects);

			CHECK(subObjects.Size() == 10);

			for(const RID& subObject: subObjects)
			{
				ResourceObject readSub = Repository::Write(subObject);
				CHECK(readSub.GetValue<i32>(TestOtherResource_TestValue) == subObject.Id);
			}
		}

		Repository::Shutdown();
		Reflection::Shutdown();
	}

	void CheckSubObjectSets(RID rid, u32 expectedCount)
	{
		Array<RID> rids{};
		ResourceObject read = Repository::Read(rid);
		usize count = read.GetSubObjectSetCount(TestResource_SubObjectSet);
		CHECK(count == expectedCount);
		rids.Resize(count);
		read.GetSubObjectSet(TestResource_SubObjectSet, rids);
		for (u32 i = 0; i < count; ++i)
		{
			ResourceObject readSub = Repository::Read(rids[i]);
			CHECK(readSub.GetValue<i32>(TestOtherResource_TestValue) == rids[i].Id);
		}
	}

	TEST_CASE("Repository::TestSubObjectsSetPrototype")
	{
		Reflection::Init();
		Repository::Init();
		CreateResourceTypes();

		RID prototype = Repository::CreateResource(TestResource);
		RID subObject1 = Repository::CreateResource(TestOtherResource);
		RID subObject2 = Repository::CreateResource(TestOtherResource);
		{
			{
				ResourceObject writeSub = Repository::Write(subObject1);
				writeSub.SetValue(TestOtherResource_TestValue, (i32) subObject1.Id);
				writeSub.Commit();
			}

			{
				ResourceObject writeSub = Repository::Write(subObject2);
				writeSub.SetValue(TestOtherResource_TestValue, (i32) subObject2.Id);
				writeSub.Commit();
			}

			ResourceObject write = Repository::Write(prototype);
			write.AddToSubObjectSet(TestResource_SubObjectSet, subObject2);
			write.AddToSubObjectSet(TestResource_SubObjectSet, subObject1);
			write.Commit();
		}


		//create new RID of prototype and check if new rid has 2 subobjects
		RID rid = Repository::CreateFromPrototype(prototype);
		CheckSubObjectSets(rid, 2);

		//remove from prototype
		{
			ResourceObject write = Repository::Write(rid);
			write.RemoveFromPrototypeSubObjectSet(TestResource_SubObjectSet, subObject1);
			write.Commit();
		}

		//check if rid has only one subobject and it's the correct one
		{
			Array<RID> rids{};
			ResourceObject read = Repository::Read(rid);
			usize count = read.GetSubObjectSetCount(TestResource_SubObjectSet);
			CHECK(count == 1);
			rids.Resize(count);
			read.GetSubObjectSet(TestResource_SubObjectSet, rids);
			CHECK(rids[0] == subObject2);
			ResourceObject readSub = Repository::Read(subObject2);
			CHECK(readSub.GetValue<i32>(TestOtherResource_TestValue) == subObject2.Id);
		}

		//check if prototype is still correct:
		CheckSubObjectSets(prototype, 2);

		//create a prototype from other prototype.
		RID rid2 = Repository::CreateFromPrototype(rid);

		//this one should still have only one item
		{
			Array<RID> rids{};
			ResourceObject read = Repository::Read(rid2);
			usize count = read.GetSubObjectSetCount(TestResource_SubObjectSet);
			CHECK(count == 1);
			rids.Resize(count);
			read.GetSubObjectSet(TestResource_SubObjectSet, rids);
			CHECK(rids[0] == subObject2);
			ResourceObject readSub = Repository::Read(subObject2);
			CHECK(readSub.GetValue<i32>(TestOtherResource_TestValue) == subObject2.Id);
		}

		{
			ResourceObject write = Repository::Write(rid);
			write.CancelRemoveFromPrototypeSubObjectSet(TestResource_SubObjectSet, subObject1);
			write.Commit();
		}

		//all items should have only one item
		CheckSubObjectSets(prototype, 2);
		CheckSubObjectSets(rid, 2);
		CheckSubObjectSets(rid2, 2);

		{
			ResourceObject write = Repository::Write(prototype);
			write.RemoveFromSubObjectSet(TestResource_SubObjectSet, subObject1);
			write.Commit();
		}

		//removed one from root prototype, all of them should have only one.
		CheckSubObjectSets(prototype, 1);
		CheckSubObjectSets(rid, 1);
		CheckSubObjectSets(rid2, 1);

		//clear subObjectSet
		{
			ResourceObject write = Repository::Write(prototype);
			write.ClearSubObjectSet(TestResource_SubObjectSet);
			write.Commit();
		}

		//all should have zero
		CheckSubObjectSets(prototype, 0);
		CheckSubObjectSets(rid, 0);
		CheckSubObjectSets(rid2, 0);

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
					for (int j = 0; j < tries * 10 || !check; ++j)
					{
						ResourceObject object = Repository::Read(rid);
						if (object)
						{
							if (object.GetValue<i64>(TestResource_LongValue) >= 0)
							{
								check = true;
							}
						}

						//never gets the value
						if (j > tries * 10000)
						{
							CHECK(false);
							break;
						}
					}
				});
			}

			for (int i = 0; i < threads; ++i)
			{
				readThreads[i].join();
				writeThreads[i].join();
			}

			CHECK(check);
		}

		Repository::Shutdown();
		Reflection::Shutdown();

		SetDefaultAllocator(*bkpDefault);
	}
}