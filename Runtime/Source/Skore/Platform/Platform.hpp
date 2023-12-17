// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"
#include "PlatformTypes.hpp"

namespace Skore::Platform
{
	SK_API void     Init();
	SK_API void     Shutdown();
	SK_API void     ProcessEvents();
	SK_API Window*  CreateWindow(const WindowCreation& windowCreation);
	SK_API bool     WindowUserRequestedClose(Window* window);
	SK_API void     DestroyWindow(Window* window);
}