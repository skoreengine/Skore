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

	AppContext* app = {};

	void App::Init()
	{
		Platform::Init();
		Repository::Init();
	}

	void App::CreateContext(const AppContextCreation& contextCreation)
	{
		app = Alloc<AppContext>();

		RenderDevice::Init(RenderDeviceType_OpenGL);
		RenderDevice::CreateDevice(RenderAdapter{});

		WindowFlags flags = 0;
		if (contextCreation.Maximized)
		{
			flags |= WindowFlags_Maximized;
		}

		if (contextCreation.Fullscreen)
		{
			flags |= WindowFlags_Fullscreen;
		}

		app->Window    = Platform::CreateWindow(contextCreation.Title, contextCreation.Size, flags);
		app->Swapchain = RenderDevice::CreateSwapchain(app->Window, contextCreation.Vsync);
		app->Running   = true;

		app->Logger.Info("Skore {} initialized ", SK_VERSION);
	}

	i32  App::Run()
	{
		while(app->Running)
		{
			Platform::ProcessEvents();
			if (Platform::UserRequestedClose(app->Window))
			{
				app->Running = false;
			}

			Extent         extent     = Platform::GetWindowExtent(app->Window);
			RenderCommands cmd        = RenderDevice::BeginFrame();

			RenderPass renderPass = RenderDevice::AcquireNextRenderPass(app->Swapchain);

			RenderDevice::BeginRenderPass(cmd, BeginRenderPassInfo{
				.RenderPass = renderPass,
				.ClearValues = {&app->ClearColor, 1}
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
			RenderDevice::EndFrame({&app->Swapchain, 1});

			Repository::GarbageCollect();
		}

		Platform::DestroyWindow(app->Window);
		RenderDevice::Shutdown();
		Repository::Shutdown();
		Platform::Shutdown();
		Reflection::Shutdown();

		Destroy(app);

		return 0;
	}
}


