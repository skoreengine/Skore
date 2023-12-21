// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "doctest.h"

#include "Skore/Core/Reflection.hpp"

namespace Skore::Tests
{

	struct ImcompleteType;

	struct ReflectionTestStruct
	{
		u32 uint;
		i32 iint;
		String string;
	};

	class ReflectionTestClass
	{
	public:

		static i32 constructorCalls;
		static i32 destructorCalls;

		ReflectionTestClass() : m_int(10), m_string("Empty")
		{
			constructorCalls++;
		}

		ReflectionTestClass(i32 anInt, const String& string) : m_int(anInt), m_string(string)
		{
			constructorCalls++;
		}

		virtual ~ReflectionTestClass()
		{
			destructorCalls++;
		}

		i32 GetInt() const
		{
			return m_int;
		}

		const String& GetString() const
		{
			return m_string;
		}

		static void ResetCount()
		{
			constructorCalls = 0;
			destructorCalls = 0;
		}

	private:
		i32 m_int;
		String m_string;
	};

	i32 ReflectionTestClass::constructorCalls = 0;
	i32 ReflectionTestClass::destructorCalls = 0;

	void TestTypeRegister()
	{
		auto testStruct = Reflection::Type<ReflectionTestStruct>();
		testStruct.Field<&ReflectionTestStruct::uint>("uint");
		testStruct.Field<&ReflectionTestStruct::iint>("iint");
		testStruct.Field<&ReflectionTestStruct::string>("string");
	}


	TEST_CASE("Core::ReflectionBasics")
	{
		Reflection::Type<ImcompleteType>();
		auto typeClass = Reflection::Type<ReflectionTestClass>();
		typeClass.Constructor<i32, String>();

		TypeHandler* testClass = Reflection::FindTypeByName("Skore::Tests::ReflectionTestClass");
		REQUIRE(testClass != nullptr);

		{
			CPtr instance = testClass->NewInstance(123, String{"TestStr"});
			CHECK(ReflectionTestClass::constructorCalls == 1);
			REQUIRE(instance != nullptr);

			ReflectionTestClass& test = *static_cast<ReflectionTestClass*>(instance);
			CHECK(test.GetInt() == 123);
			CHECK(test.GetString() == "TestStr");

			testClass->Destroy(instance);
			CHECK(ReflectionTestClass::destructorCalls == 1);

			ReflectionTestClass::ResetCount();
		}

		{
			CPtr instance = testClass->NewInstance();
			CHECK(ReflectionTestClass::constructorCalls == 1);
			REQUIRE(instance != nullptr);

			ReflectionTestClass& test = *static_cast<ReflectionTestClass*>(instance);
			CHECK(test.GetInt() == 10);
			CHECK(test.GetString() == "Empty");

			testClass->Destroy(instance);
			CHECK(ReflectionTestClass::destructorCalls == 1);
			ReflectionTestClass::ResetCount();
		}

		TypeHandler* nullRet = Reflection::FindTypeByName("ClassNameThatDontExists");
		CHECK(nullRet == nullptr);

		Reflection::Shutdown();
	}

	TEST_CASE("Core::ReflectionFields")
	{
		TestTypeRegister();
		TypeHandler* testStruct = Reflection::FindTypeByName("Skore::Tests::ReflectionTestStruct");
		REQUIRE(testStruct != nullptr);

		REQUIRE(testStruct->GetConstructors().Size() == 1);

		FieldHandler* uintField = testStruct->FindField("uint");
		FieldHandler* iintField = testStruct->FindField("iint");
		FieldHandler* stringField = testStruct->FindField("string");

		REQUIRE(uintField != nullptr);
		REQUIRE(iintField != nullptr);
		REQUIRE(stringField != nullptr);

		Span<FieldHandler*> fields = testStruct->GetFields();
		CHECK(fields.Size() == 3);

		CHECK(GetTypeID<u32>() == uintField->GetFieldInfo().typeInfo.typeId);
		CHECK(GetTypeID<String>() == stringField->GetFieldInfo().typeInfo.typeId);

		CPtr instance = testStruct->NewInstance();
		REQUIRE(instance != nullptr);

		static_cast<ReflectionTestStruct*>(instance)->iint = 100;

		CHECK(iintField->GetFieldPointer(instance) != nullptr);
		CHECK(iintField->GetFieldAs<i32>(instance) == 100);

		uintField->SetValueAs(instance, 10u);

		u32 vlCopy{};
		uintField->CopyValueTo(instance, &vlCopy);
		CHECK(vlCopy == 10);
		testStruct->Destroy(instance);

		Reflection::Shutdown();
	}
}