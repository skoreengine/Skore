// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once
#include "Defines.hpp"
#include "Skore/Core/String.hpp"
#include "Skore/Math/MathTypes.hpp"

namespace Skore
{
	struct AppContextCreation
	{
		String Title      = "Skore";
		UVec2  Size       = UVec2{1920, 1080};
		bool   Maximized  = false;
		bool   Fullscreen = false;
		bool   Vsync      = true;

	};

	namespace App
	{
		void SK_API Init();
		void SK_API CreateContext(const AppContextCreation& contextCreation);
		i32  SK_API Run();
	}
}