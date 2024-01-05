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
		bool GladLoaded = false;
	};

	GLContext gl;

	void Init()
	{
		Platform::InitOpenGL();
	}

	void CreateDevice(RenderAdapter adapter)
	{
		//do nothing on OpenGL
	}

	void Shutdown()
	{

	}

	RenderSwapchain CreateSwapchain(Window window, bool vsync)
	{
		Platform::MakeContextCurrent(window);
#ifdef SK_DESKTOP
		if (!gl.GladLoaded && !gladLoadGLLoader((GLADloadproc) Platform::GetProcAddress()))
		{
			gl.Logger.Error("[GL] Failed to initialize GLAD");
		}
		gl.GladLoaded = true;
#endif
		Platform::SetVSync(vsync);
		return {new GLSwapchain{window, GLRenderPass{0, window}}};
	}

	RenderPass AcquireNextRenderPass(RenderSwapchain swapchain)
	{
		GLSwapchain* glSwapchain = static_cast<GLSwapchain*>(swapchain.Handler);
		return {&glSwapchain->RenderPass};
	}

	void DestroySwapchain(RenderSwapchain swapchain)
	{
		delete static_cast<GLSwapchain*>(swapchain.Handler);
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
		GLRenderPass* glRenderPass = static_cast<GLRenderPass*>(beginRenderPassInfo.RenderPass.Handler);

		if (glRenderPass->FramebufferId == 0)
		{
			Platform::MakeContextCurrent(glRenderPass->WindowContext);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, glRenderPass->FramebufferId);

		Vec4 cleanColor = beginRenderPassInfo.ClearValues[0];
		glClearColor(cleanColor.X, cleanColor.Y, cleanColor.Z, cleanColor.W);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

	void EndFrame(const Span<RenderSwapchain>& swapchains)
	{
		for(const RenderSwapchain& swapchain : swapchains)
		{
			GLSwapchain* glSwapchain = static_cast<GLSwapchain*>(swapchain.Handler);
			Platform::MakeContextCurrent(glSwapchain->Window);
			Platform::SwapBuffers(glSwapchain->Window);
		}
	}

}

namespace Skore
{
	void RegisterOpenGL(RenderDeviceAPI& renderDeviceApi)
	{
		renderDeviceApi.Init                         = GL::Init;
		renderDeviceApi.Shutdown                     = GL::Shutdown;
		renderDeviceApi.CreateDevice                 = GL::CreateDevice;
		renderDeviceApi.CreateSwapchain              = GL::CreateSwapchain;
		renderDeviceApi.CreateGraphicsPipelineState  = GL::CreateGraphicsPipelineState;
		renderDeviceApi.DestroyGraphicsPipelineState = GL::DestroyGraphicsPipelineState;
		renderDeviceApi.DestroySwapchain             = GL::DestroySwapchain;
		renderDeviceApi.BeginRenderPass              = GL::BeginRenderPass;
		renderDeviceApi.EndRenderPass                = GL::EndRenderPass;
		renderDeviceApi.SetViewport                  = GL::SetViewport;
		renderDeviceApi.SetScissor                   = GL::SetScissor;
		renderDeviceApi.BeginFrame                   = GL::BeginFrame;
		renderDeviceApi.AcquireNextRenderPass        = GL::AcquireNextRenderPass;
		renderDeviceApi.EndFrame                     = GL::EndFrame;
	}
}
