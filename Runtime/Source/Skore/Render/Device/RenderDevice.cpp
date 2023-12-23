// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "RenderDevice.hpp"
#include "RenderDeviceAPI.hpp"

namespace Skore
{
	RenderDeviceAPI renderDeviceApi{};

	void RegisterOpenGL(RenderDeviceAPI& renderDeviceApi);

	void RenderDevice::Init(const AppCreation& creation)
	{
//		if (creation.Device == RenderDeviceType_OpenGL)
//		{
//
//		}
		RegisterOpenGL(renderDeviceApi);
		renderDeviceApi.Init();
	}

	void RenderDevice::Shutdown()
	{
		renderDeviceApi.Shutdown();
	}

	RenderSwapchain RenderDevice::CreateSwapchain(Window window, bool vsync)
	{
		return renderDeviceApi.CreateSwapchain(window, vsync);
	}

	RenderPipelineState RenderDevice::CreateGraphicsPipelineState(const GraphicsPipelineCreation& graphicsPipelineCreation)
	{
		return renderDeviceApi.CreateGraphicsPipelineState(graphicsPipelineCreation);
	}

	void RenderDevice::DestroyGraphicsPipelineState(RenderPipelineState pipelineState)
	{
		renderDeviceApi.DestroyGraphicsPipelineState(pipelineState);
	}

	void RenderDevice::DestroySwapchain(RenderSwapchain swapchain)
	{
		renderDeviceApi.DestroySwapchain(swapchain);
	}

	void RenderDevice::BeginRenderPass(RenderCommands cmd, const BeginRenderPassInfo& beginRenderPassInfo)
	{
		renderDeviceApi.BeginRenderPass(cmd, beginRenderPassInfo);
	}

	void RenderDevice::EndRenderPass(RenderCommands cmd)
	{
		renderDeviceApi.EndRenderPass(cmd);
	}

	void RenderDevice::SetViewport(RenderCommands cmd, const ViewportInfo& viewportInfo)
	{
		renderDeviceApi.SetViewport(cmd, viewportInfo);
	}

	void RenderDevice::SetScissor(RenderCommands cmd, const Rect& rect)
	{
		renderDeviceApi.SetScissor(cmd, rect);
	}

	RenderCommands RenderDevice::BeginFrame()
	{
		return renderDeviceApi.BeginFrame();
	}

	void RenderDevice::EndFrame(RenderSwapchain swapchain)
	{
		renderDeviceApi.EndFrame(swapchain);
	}
}