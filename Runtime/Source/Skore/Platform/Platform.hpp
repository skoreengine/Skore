#pragma once

#include "Skore/Defines.hpp"
#include "PlatformTypes.hpp"
#include "Skore/Math/MathTypes.hpp"
#include "Skore/Core/StringView.hpp"

namespace Skore::Platform
{
	SK_API void Init();
	SK_API void Shutdown();

	SK_API void         ProcessEvents();
	SK_API Window*      CreateWindow(const StringView& title, UVec2 size, WindowFlags flags);
	SK_API Extent       GetWindowExtent(Window* window);
	SK_API bool         UserRequestedClose(Window* window);
	SK_API void         DestroyWindow(Window* window);

	SK_API CPtr         LoadSharedLib(const StringView& libPath);
	SK_API CPtr         GetAddress(CPtr, const char* funcName);

	SK_API f64          GetTime();
	SK_API void         ConsoleWrite(const StringView& message);
}