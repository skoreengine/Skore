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
		GLFWwindow* handler{};
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

		bool maximized = (windowCreation.windowFlags & WindowFlags_Maximized);
		glfwWindowHint(GLFW_MAXIMIZED, maximized);

		float xScale = 1.f, yScale = 1.f;
		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xScale, &yScale);
		UVec2 size = {u32(windowCreation.rect.width * xScale), u32(windowCreation.rect.height * yScale)};

		window->handler = glfwCreateWindow(size.width, size.height, windowCreation.title.CStr(), nullptr, nullptr);

		ApplyDarkStyle(window->handler);
		glfwShowWindow(window->handler);


		return window;
	}

	bool Platform::WindowUserRequestedClose(Window* window)
	{
		return glfwWindowShouldClose(window->handler);
	}

	void Platform::DestroyWindow(Window* window)
	{
		glfwDestroyWindow(window->handler);
		Destroy(window);
	}
}

#endif