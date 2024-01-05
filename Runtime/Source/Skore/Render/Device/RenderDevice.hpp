// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "Skore/App.hpp"
#include "Skore/Math/MathTypes.hpp"
#include "Skore/Render/RenderTypes.hpp"
#include "Skore/Platform/PlatformTypes.hpp"

namespace Skore::RenderDevice
{
	SK_API void                   Init(const RenderDeviceType& renderDeviceType);
	SK_API void                   Shutdown();
	SK_API Span<RenderAdapter>    GetAdapters();
	SK_API void                   CreateDevice(RenderAdapter adapter);

	SK_API RenderSwapchain        CreateSwapchain(Window window, bool vsync);
	SK_API RenderPipelineState    CreateGraphicsPipelineState(const GraphicsPipelineCreation& graphicsPipelineCreation);

	SK_API void                   DestroyGraphicsPipelineState(RenderPipelineState pipelineState);
	SK_API void                   DestroySwapchain(RenderSwapchain swapchain);

	SK_API void                   BeginRenderPass(RenderCommands cmd, const BeginRenderPassInfo& beginRenderPassInfo);
	SK_API void                   EndRenderPass(RenderCommands cmd);
	SK_API void                   SetViewport(RenderCommands cmd, const ViewportInfo& viewportInfo);
	SK_API void                   SetScissor(RenderCommands cmd, const Rect& rect);

	SK_API RenderCommands         BeginFrame();
	SK_API RenderPass             AcquireNextRenderPass(RenderSwapchain swapchain);
	SK_API void                   EndFrame(const Span<RenderSwapchain>& swapchains);

}
