// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Defines.hpp"

namespace Skore::Platform
{
	SK_API void InitOpenGL();
	SK_API void SetVSync(bool vsync);
	SK_API void MakeContextCurrent(Window window);
	SK_API void SwapBuffers(Window window);
	SK_API CPtr GetProcAddress();
}