// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "App.hpp"
#include "Skore/Platform/Platform.hpp"
#include "Skore/Core/Logger.hpp"
#include "Skore/Core/Reflection.hpp"
#include "Skore/Render/Device/RenderDevice.hpp"
#include "Skore/Repository/Repository.hpp"

namespace Skore
{
	struct AppContext
	{
		bool            Running    = false;
		Window          Window{};
		RenderSwapchain Swapchain{};
		Vec4            ClearColor = Vec4{0, 0, 0, 1};
		Logger& Logger = Logger::GetLogger("Skore::App");
	};

	AppContext app = {};

	void App::Init(const AppCreation& appCreation)
	{
		Platform::Init();
		Repository::Init();
		RenderDevice::Init(appCreation);

		WindowFlags flags = 0;
		if (appCreation.Fullscreen)
		{
			flags |= WindowFlags_Maximized;
		}

		app.Window    = Platform::CreateWindow(appCreation.Title, appCreation.Size, flags);
		app.Swapchain = RenderDevice::CreateSwapchain(app.Window, appCreation.Vsync);
		app.Running   = true;

		app.Logger.Info("Skore {} initialized ", SK_VERSION);
	}

	bool App::Update()
	{
		Platform::ProcessEvents();

		if (Platform::UserRequestedClose(app.Window))
		{
			app.Running = false;
		}

		Extent         extent = Platform::GetWindowExtent(app.Window);
		RenderCommands cmd    = RenderDevice::BeginFrame();

		RenderDevice::BeginRenderPass(cmd, BeginRenderPassInfo{
			.Swapchain = app.Swapchain,
			.ClearValues = {&app.ClearColor, 1}
		});

		ViewportInfo viewportInfo{};
		viewportInfo.X        = 0.;
		viewportInfo.Y        = 0.;
		viewportInfo.Width    = (f32) extent.Width;
		viewportInfo.Height   = (f32) extent.Height;
		viewportInfo.MaxDepth = 0.;
		viewportInfo.MinDepth = 1.;
		RenderDevice::SetViewport(cmd, viewportInfo);

		auto scissor = Rect{0, 0, extent.Width, extent.Height};
		RenderDevice::SetScissor(cmd, scissor);

		RenderDevice::EndRenderPass(cmd);
		RenderDevice::EndFrame(app.Swapchain);

		return app.Running;
	}

	void App::Shutdown()
	{
		Platform::DestroyWindow(app.Window);
		Platform::Shutdown();
		Repository::Shutdown();
		Reflection::Shutdown();
	}
}


