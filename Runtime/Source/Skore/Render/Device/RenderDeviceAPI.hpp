// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Render/RenderTypes.hpp"
#include "Skore/Platform/PlatformTypes.hpp"
#include "Skore/Math/MathTypes.hpp"

namespace Skore
{
	struct RenderDeviceAPI
	{
		void (*Init)();
		void (*Shutdown)();

		RenderSwapchain         (*CreateSwapchain)(Window* window, bool vsync);
		void                    (*DestroySwapchain)(RenderSwapchain swapchain);
		RenderPipelineState     (*CreateGraphicsPipelineState)(const GraphicsPipelineCreation& graphicsPipelineCreation);
		void                    (*DestroyGraphicsPipelineState)(RenderPipelineState renderPipelineState);

		void                    (*BeginRenderPass)(RenderCommands cmd, const BeginRenderPassInfo& beginRenderPassInfo);
		void                    (*EndRenderPass)(RenderCommands cmd);
		void                    (*SetViewport)(RenderCommands cmd, const ViewportInfo& viewportInfo);
		void                    (*SetScissor)(RenderCommands cmd, const Rect& rect);

		RenderCommands          (*BeginFrame)();
		void                    (*EndFrame)(RenderSwapchain swapchain);
	};
}