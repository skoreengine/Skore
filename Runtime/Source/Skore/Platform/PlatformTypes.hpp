// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

#include "Skore/Core/StringView.hpp"
#include "Skore/Math/MathTypes.hpp"
namespace Skore
{
	struct Window;

	enum WindowFlags_
	{
		WindowFlags_None      = 0,
		WindowFlags_Maximized = 1 << 0
	};

	using WindowFlags = i32;

	struct WindowCreation
	{
		StringView  title       = "";
		Rect        rect        = {0,0,800, 600};
		WindowFlags windowFlags = WindowFlags_None;
	};
}