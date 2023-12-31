//   Copyright 2023 Paulo Marangoni.
//   Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "PlatformTypes.hpp"
#include "Skore/Defines.hpp"
#include "PlatformStyle.hpp"

#ifdef SK_WIN

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

enum WindowCompositionAttrib
{
	WCA_UNDEFINED = 0,
	WCA_NCRENDERING_ENABLED = 1,
	WCA_NCRENDERING_POLICY = 2,
	WCA_TRANSITIONS_FORCEDISABLED = 3,
	WCA_ALLOW_NCPAINT = 4,
	WCA_CAPTION_BUTTON_BOUNDS = 5,
	WCA_NONCLIENT_RTL_LAYOUT = 6,
	WCA_FORCE_ICONIC_REPRESENTATION = 7,
	WCA_EXTENDED_FRAME_BOUNDS = 8,
	WCA_HAS_ICONIC_BITMAP = 9,
	WCA_THEME_ATTRIBUTES = 10,
	WCA_NCRENDERING_EXILED = 11,
	WCA_NCADORNMENTINFO = 12,
	WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
	WCA_VIDEO_OVERLAY_ACTIVE = 14,
	WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
	WCA_DISALLOW_PEEK = 16,
	WCA_CLOAK = 17,
	WCA_CLOAKED = 18,
	WCA_ACCENT_POLICY = 19,
	WCA_FREEZE_REPRESENTATION = 20,
	WCA_EVER_UNCLOAKED = 21,
	WCA_VISUAL_OWNER = 22,
	WCA_HOLOGRAPHIC = 23,
	WCA_EXCLUDED_FROM_DDA = 24,
	WCA_PASSIVEUPDATEMODE = 25,
	WCA_USEDARKMODECOLORS = 26,
	WCA_LAST = 27
};

struct WindowCompositionAttribData
{
	WindowCompositionAttrib attrib;
	PVOID pvData;
	SIZE_T cbData;
};

using rtlGetNtVersionNumbersFn = void (WINAPI*)(LPDWORD major, LPDWORD minor, LPDWORD build);
using allowDarkModeForWindowFn = bool (WINAPI*)(HWND hWnd, bool allow);
using setWindowCompositionAttributeFn = BOOL (WINAPI*)(HWND hWnd, WindowCompositionAttribData*);

static allowDarkModeForWindowFn g_allowDarkModeForWindow = nullptr;
static setWindowCompositionAttributeFn g_setWindowCompositionAttribute = nullptr;
static bool g_darkModeAllowed{};

namespace Skore
{
	void PlatformStyle::Init()
	{
		auto rtlGetNtVersionNumbers = reinterpret_cast<rtlGetNtVersionNumbersFn>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetNtVersionNumbers"));
		if (rtlGetNtVersionNumbers)
		{
			DWORD major, minor, buildNumber = 0;
			rtlGetNtVersionNumbers(&major, &minor, &buildNumber);
			if (major >= 10 && buildNumber >= 17763)
			{
				HMODULE hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
				if (hUxtheme)
				{
					g_allowDarkModeForWindow = reinterpret_cast<allowDarkModeForWindowFn>(GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133)));
					g_setWindowCompositionAttribute = reinterpret_cast<setWindowCompositionAttributeFn>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetWindowCompositionAttribute"));

					if (g_allowDarkModeForWindow && g_setWindowCompositionAttribute)
					{
						g_darkModeAllowed = true;
					}

				}
			}
		}
	}

	void PlatformStyle::ApplyDarkStyle(CPtr internal)
	{
		HWND hwnd = glfwGetWin32Window(static_cast<GLFWwindow*>(internal));
		if (g_darkModeAllowed)
		{
			g_allowDarkModeForWindow(hwnd, true);
			BOOL dark = TRUE;
			WindowCompositionAttribData data = {WCA_USEDARKMODECOLORS, &dark, sizeof(dark)};
			g_setWindowCompositionAttribute(hwnd, &data);
		}
	}

}

#endif