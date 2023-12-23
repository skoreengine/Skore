// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Platform.hpp"
#include "Skore/Core/Allocator.hpp"

#ifdef SK_DESKTOP

#include <GLFW/glfw3.h>

namespace Skore
{
	namespace Platform
	{
		void InitStyle();
		void ApplyDarkStyle(CPtr internal);
	}


	struct Window
	{
		GLFWwindow* Handler{};
	};

	void Platform::Init()
	{
		glfwInit();
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		InitStyle();
	}

	void Platform::Shutdown()
	{
		glfwTerminate();
	}

	void Platform::ProcessEvents()
	{
		glfwPollEvents();
	}

	Window* Platform::CreateWindow(const WindowCreation& windowCreation)
	{
		Window* window = Alloc<Window>();

		bool maximized = (windowCreation.WindowFlags & WindowFlags_Maximized);
		glfwWindowHint(GLFW_MAXIMIZED, maximized);

		float xScale = 1.f, yScale = 1.f;
		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xScale, &yScale);
		UVec2 size = {u32(windowCreation.Rect.width * xScale), u32(windowCreation.Rect.height * yScale)};

		window->Handler = glfwCreateWindow(size.width, size.height, windowCreation.Title.CStr(), nullptr, nullptr);

		ApplyDarkStyle(window->Handler);
		glfwShowWindow(window->Handler);

		return window;
	}

	bool Platform::WindowUserRequestedClose(Window* window)
	{
		return glfwWindowShouldClose(window->Handler);
	}

	void Platform::DestroyWindow(Window* window)
	{
		glfwDestroyWindow(window->Handler);
		Destroy(window);
	}
}

#endif