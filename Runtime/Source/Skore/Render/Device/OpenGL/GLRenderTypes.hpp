// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

namespace Skore::GL
{
	struct GLRenderPass
	{
		u32    FramebufferId = 0;
		Window WindowContext{};
	};

	struct GLSwapchain
	{
		Window Window{};
		GLRenderPass RenderPass{};
	};
}