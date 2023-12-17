// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "App.hpp"
#include "Skore/Platform/Platform.hpp"
#include <iostream>
#include "Skore/Core/String.hpp"
#include "Skore/Core/Array.hpp"

namespace Skore
{
	struct AppContext
	{
		bool running = false;
	};

	AppContext appContext = {};

	void App::Init()
	{

		Array<i32> arrInt{};
		arrInt.EmplaceBack(1);
		arrInt.EmplaceBack(2);
		arrInt.EmplaceBack(3);


		Array<String> arrStr{};
		arrStr.Reserve(1000);
		arrStr.EmplaceBack("blah1");
		arrStr.EmplaceBack("blah2");
		arrStr.EmplaceBack("blah3");
		//arrStr.EmplaceBack("blah4");
		arrStr.EmplaceBack("blah5");
		arrStr.EmplaceBack("blah6");
		arrStr.EmplaceBack("blah7");

		{
			Array<String> arrStr2{};
			arrStr2.EmplaceBack("blah10");
			arrStr2.EmplaceBack("blah20");
			arrStr2.EmplaceBack("blah30");
			arrStr2.EmplaceBack("blah50");

			arrStr.Insert(arrStr.begin() + 3, arrStr2.begin(), arrStr2.end());
		}


		for (const String& str: arrStr)
		{
			std::wcout << str.CStr() << std::endl;
		}

		Platform::Init();

		appContext.running = false;
//		std::cout << "Hello, Skore!" << std::endl;
	}

	bool App::Update()
	{
		return appContext.running;
	}

	void App::Shutdown()
	{
		//std::cout << "Shutdown, Skore!" << std::endl;
		appContext = {};
	}
}


