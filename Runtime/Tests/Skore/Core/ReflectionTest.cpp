// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "doctest.h"

#include "Skore/Core/Reflection.hpp"

namespace Skore::Tests
{

	struct ImcompleteType;

	struct ReflectionTestStruct
	{
		u32    Uint;
		i32    Iint;
		String String;
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

	struct ReflectionFunctions
	{
		static i32 StaticVoidFuncCalls;
		i32        Calls = 0;

		void VoidFunc()
		{
			Calls++;
		}

		i32 ParamsRetFunc(i32 a, i32 b)
		{
			Calls++;
			return a + b;
		}

		static i32 StaticFunc(i32 a, i32 b)
		{
			StaticVoidFuncCalls++;
			return a * b;
		}

		static void StaticFuncNoParam()
		{
			StaticVoidFuncCalls++;
		}

	};

	i32 ReflectionTestClass::constructorCalls = 0;
	i32 ReflectionTestClass::destructorCalls = 0;
	i32 ReflectionFunctions::StaticVoidFuncCalls = 0;

	void TestTypeRegister()
	{
		auto testStruct = Reflection::Type<ReflectionTestStruct>();
		testStruct.Field<&ReflectionTestStruct::Uint>("Uint");
		testStruct.Field<&ReflectionTestStruct::Iint>("Iint");
		testStruct.Field<&ReflectionTestStruct::String>("String");
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

		FieldHandler* uintField = testStruct->FindField("Uint");
		FieldHandler* iintField = testStruct->FindField("Iint");
		FieldHandler* stringField = testStruct->FindField("String");

		REQUIRE(uintField != nullptr);
		REQUIRE(iintField != nullptr);
		REQUIRE(stringField != nullptr);

		Span<FieldHandler*> fields = testStruct->GetFields();
		CHECK(fields.Size() == 3);

		CHECK(GetTypeID<u32>() == uintField->GetFieldInfo().TypeInfo.TypeId);
		CHECK(GetTypeID<String>() == stringField->GetFieldInfo().TypeInfo.TypeId);

		CPtr instance = testStruct->NewInstance();
		REQUIRE(instance != nullptr);

		static_cast<ReflectionTestStruct*>(instance)->Iint = 100;

		CHECK(iintField->GetFieldPointer(instance) != nullptr);
		CHECK(iintField->GetFieldAs<i32>(instance) == 100);

		uintField->SetValueAs(instance, 10u);

		u32 vlCopy{};
		uintField->CopyValueTo(instance, &vlCopy);
		CHECK(vlCopy == 10);
		testStruct->Destroy(instance);

		Reflection::Shutdown();
	}

	TEST_CASE("Core::ReflectionFunctions")
	{
		{
			auto reflectionFunctions = Reflection::Type<ReflectionFunctions>();
			reflectionFunctions.Function<&ReflectionFunctions::VoidFunc>("VoidFunc");
			reflectionFunctions.Function<&ReflectionFunctions::ParamsRetFunc>("ParamsRetFunc");
			reflectionFunctions.Function<&ReflectionFunctions::StaticFunc>("StaticFunc");
			reflectionFunctions.Function<&ReflectionFunctions::StaticFuncNoParam>("StaticFuncNoParam");
		}

		TypeHandler* handler = Reflection::FindType<ReflectionFunctions>();
		REQUIRE(handler != nullptr);
		FunctionHandler* voidFunc = handler->FindFunction("VoidFunc");
		REQUIRE(voidFunc != nullptr);

		CHECK(voidFunc->GetName() == "VoidFunc");
		CHECK(voidFunc->GetParams().Empty());
		CHECK(voidFunc->GetReturn().TypeInfo.TypeId == GetTypeID<void>());

		ReflectionFunctions reflectionFunctions{};
		voidFunc->Call(&reflectionFunctions, nullptr, nullptr);
		CHECK(reflectionFunctions.Calls == 1);

		typedef void(*FnVoidFunc)(const FunctionHandler* handler, CPtr instance);
		FnVoidFunc func = reinterpret_cast<FnVoidFunc>(voidFunc->GetFunctionPointer());
		func(voidFunc, &reflectionFunctions);
		CHECK(reflectionFunctions.Calls == 2);

		FunctionHandler* paramRetFunc = handler->FindFunction("ParamsRetFunc");
		REQUIRE(paramRetFunc != nullptr);
		CHECK(paramRetFunc->GetReturn().TypeInfo.TypeId == GetTypeID<i32>());
		CHECK(!paramRetFunc->GetParams().Empty());

		{
			i32 ret = 0;
			i32 a = 20;
			i32 b = 10;
			CPtr params[] = {&a, &b};
			paramRetFunc->Call(&reflectionFunctions, &ret, params);
			CHECK(reflectionFunctions.Calls == 3);
			CHECK(ret == 30);
		}


		FunctionHandler* staticFunc = handler->FindFunction("StaticFunc");
		REQUIRE(staticFunc != nullptr);
		{
			i32 ret = 0;
			i32 a = 20;
			i32 b = 10;
			CPtr params[] = {&a, &b};
			staticFunc->Call(nullptr, &ret, params);
			CHECK(reflectionFunctions.Calls == 3);
			CHECK(ret == 200);
			CHECK(ReflectionFunctions::StaticVoidFuncCalls == 1);
		}

		FunctionHandler* staticFuncNoParam = handler->FindFunction("StaticFuncNoParam");
		REQUIRE(staticFuncNoParam != nullptr);
		staticFuncNoParam->Call(nullptr, nullptr, nullptr);
		CHECK(ReflectionFunctions::StaticVoidFuncCalls == 2);

		Reflection::Shutdown();
	}
}