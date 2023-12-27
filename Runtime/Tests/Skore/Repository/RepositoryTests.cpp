// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "doctest.h"
#include "Skore/Repository/Repository.hpp"
#include "Skore/Core/Reflection.hpp"


namespace Skore::Tests
{
	struct SimpleTest
	{
		i32        Value{};
		Array<RID> SubObjects{};
	};

	TEST_CASE("Repository::Basics")
	{
		Repository::Init();

//		auto runtimeType = Reflection::Type("Skore::Tests::RuntimeTestTypeID", RuntimeTestTypeId);
//		runtimeType.Field("Value", GetTypeID<i32>());
//
//		auto simpleTest = Reflection::Type<SimpleTest>();
//		simpleTest.Field<&SimpleTest::Value>("Value");
//
//
//		RID rid = Repository::CreateObject(GetTypeID<SimpleTest>());
//
//		SimpleTest* write = Repository::Write<SimpleTest>(rid);
//		write->Value = 10;
//		Repository::Commit(rid, write);
//
//		const SimpleTest* read = Repository::Read<SimpleTest>(rid);
//		CHECK(read->Value == 10);


//
//		RID rid = Repository::CreateObject(GetTypeID<SimpleTest>());
//		ResourceObject writeObject = Repository::Write(rid);
//		writeObject.SetValue("Value", 10);
//		writeObject.Commit();
//
//		ResourceObject readObject = Repository::Get(rid);
//		CHECK(readObject.GetValueAs<i32>("Value") == 10);


		Reflection::Shutdown();
		Repository::Shutdown();
	}
}