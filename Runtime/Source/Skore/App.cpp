// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "App.hpp"
#include "Skore/Platform/Platform.hpp"
#include "Skore/Core/Logger.hpp"
#include "Skore/Core/Reflection.hpp"

namespace Skore
{
	struct AppContext
	{
		bool running = false;
		Window* window = nullptr;
		Logger& logger = Logger::GetLogger("Skore::App");
	};

	AppContext app = {};

	void App::Init()
	{
		Platform::Init();

		WindowCreation windowCreation{
			.title = "Skore",
			.windowFlags = WindowFlags_Maximized
		};

		app.window  = Platform::CreateWindow(windowCreation);
		app.running = true;

		app.logger.Info("Skore {} initialized ", SK_VERSION);
	}

	bool App::Update()
	{
		Platform::ProcessEvents();

		if (Platform::WindowUserRequestedClose(app.window))
		{
			app.running = false;
		}
		return app.running;
	}

	void App::Shutdown()
	{
		Platform::DestroyWindow(app.window);
		Platform::Shutdown();
		Reflection::Shutdown();
	}
}


