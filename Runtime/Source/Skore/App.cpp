// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "App.hpp"
#include <iostream>

namespace Skore
{
	struct AppContext
	{
		bool running = false;
	};

	AppContext appContext = {};

	void App::Init()
	{
		appContext.running = true;
		std::cout << "Hello, Skore!" << std::endl;
	}

	bool App::Update()
	{
		return appContext.running;
	}

	void App::Shutdown()
	{
		std::cout << "Shutdown, Skore!" << std::endl;
		appContext = {};
	}
}


