// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "App.hpp"
#include "Skore/Platform/Platform.hpp"
//#include <iostream>
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
		String myString = {TEXT("blah")};
		myString.Append(TEXT("ab"));

		Array<String> arrString{};
		arrString.Resize(50);
		arrString.Clear();

		arrString.EmplaceBack(String{});



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


