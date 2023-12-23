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
		bool Running = false;
		Window* Window = nullptr;
		Logger& Logger = Logger::GetLogger("Skore::App");
	};

	AppContext app = {};

	void App::Init()
	{
		Platform::Init();

		WindowCreation windowCreation{
			.Title = "Skore",
			.WindowFlags = WindowFlags_Maximized
		};

		app.Window  = Platform::CreateWindow(windowCreation);
		app.Running = true;
		app.Logger.Info("Skore {} initialized ", SK_VERSION);
	}

	bool App::Update()
	{
		Platform::ProcessEvents();

		if (Platform::WindowUserRequestedClose(app.Window))
		{
			app.Running = false;
		}
		return app.Running;
	}

	void App::Shutdown()
	{
		Platform::DestroyWindow(app.Window);
		Platform::Shutdown();
		Reflection::Shutdown();
	}
}


