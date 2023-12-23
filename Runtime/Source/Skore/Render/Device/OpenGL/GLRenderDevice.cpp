// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Skore/Render/Device/RenderDeviceAPI.hpp"
#include "GLPlatform.hpp"
#include "GLRenderTypes.hpp"

#ifdef SK_DESKTOP
#include "glad/glad.h"
#include "Skore/Core/Logger.hpp"
#endif

namespace Skore::GL
{

	struct GLContext
	{
		Logger& Logger = Logger::GetLogger("Skore::GL");
		bool gladLoaded = false;
	};

	GLContext gl;

	void Init()
	{
		Platform::InitOpenGL();
	}

	void Shutdown()
	{

	}

	RenderSwapchain CreateSwapchain(Window* window, bool vsync)
	{
		Platform::MakeContextCurrent(window);
#ifdef SK_DESKTOP
		if (!gl.gladLoaded && !gladLoadGLLoader((GLADloadproc) Platform::GetProcAddress()))
		{
			gl.Logger.Error("[GL] Failed to initialize GLAD");
		}
		gl.gladLoaded = true;
#endif
		Platform::SetVSync(vsync);
		return {new GLSwapchain{window}};
	}

	void DestroySwapchain(RenderSwapchain swapchain)
	{
		delete static_cast<GLSwapchain*>(swapchain.handler);
	}

	RenderPipelineState CreateGraphicsPipelineState(const GraphicsPipelineCreation& graphicsPipelineCreation)
	{
		return {};
	}

	void DestroyGraphicsPipelineState(RenderPipelineState pipelineState)
	{

	}

	void BeginRenderPass(RenderCommands cmd, const BeginRenderPassInfo& beginRenderPassInfo)
	{
		if (beginRenderPassInfo.Swapchain)
		{
			Vec4 cleanColor = beginRenderPassInfo.ClearValues[0];
			glClearColor(cleanColor.X, cleanColor.Y, cleanColor.Z, cleanColor.W);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		else if (beginRenderPassInfo.RenderPass)
		{

		}
	}

	void EndRenderPass(RenderCommands cmd)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void SetViewport(RenderCommands cmd, const ViewportInfo& viewportInfo)
	{
		glViewport(viewportInfo.X, viewportInfo.Y, viewportInfo.Width, viewportInfo.Height);
	}

	void SetScissor(RenderCommands cmd, const Rect& rect)
	{
		glScissor(rect.X, rect.Y, rect.Width, rect.Height);
	}

	RenderCommands BeginFrame()
	{
		return {};
	}

	void EndFrame(RenderSwapchain swapchain)
	{
		Platform::SwapBuffers(static_cast<GLSwapchain*>(swapchain.handler)->window);
	}

}

namespace Skore
{
	void RegisterOpenGL(RenderDeviceAPI& renderDeviceApi)
	{
		renderDeviceApi.Init                         = GL::Init;
		renderDeviceApi.Shutdown                     = GL::Shutdown;
		renderDeviceApi.CreateSwapchain              = GL::CreateSwapchain;
		renderDeviceApi.CreateGraphicsPipelineState  = GL::CreateGraphicsPipelineState;
		renderDeviceApi.DestroyGraphicsPipelineState = GL::DestroyGraphicsPipelineState;
		renderDeviceApi.DestroySwapchain             = GL::DestroySwapchain;
		renderDeviceApi.BeginRenderPass              = GL::BeginRenderPass;
		renderDeviceApi.EndRenderPass                = GL::EndRenderPass;
		renderDeviceApi.SetViewport                  = GL::SetViewport;
		renderDeviceApi.SetScissor                   = GL::SetScissor;
		renderDeviceApi.BeginFrame                   = GL::BeginFrame;
		renderDeviceApi.EndFrame                     = GL::EndFrame;
	}
}
