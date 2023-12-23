// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once
#include "Defines.hpp"
#include "Skore/Core/String.hpp"
#include "Skore/Math/MathTypes.hpp"

namespace Skore
{
	struct AppCreation
	{
		String Title      = "Skore";
		UVec2  Size       = UVec2{1920, 1080};
		bool   Fullscreen = false;
		bool   Vsync      = true;
	};

	namespace App
	{
		void SK_API Init(const AppCreation& appCreation);
		bool SK_API Update();
		void SK_API Shutdown();
	}
}