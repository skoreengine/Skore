#include "Platform.hpp"

#ifdef SK_DESKTOP

#include "Skore/Core/Logger.hpp"
#include "Skore/Render/Device/OpenGL/GLPlatform.hpp"
#include "PlatformStyle.hpp"
#include <GLFW/glfw3.h>

namespace Skore
{

	struct PlatformContext
	{
		Logger& Logger = Logger::GetLogger("Skore::PlatformGLFW");
	};

	PlatformContext platform{};

	struct Window
	{
		GLFWwindow* handler;
	};

	void Platform::Init()
	{
		if (!glfwInit())
		{
			platform.Logger.Error("Error on initialize GLFW");
			return;
		}

		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		PlatformStyle::Init();
	}

	void Platform::InitOpenGL()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef SK_APPLE
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	}

	void Platform::SetVSync(bool vsync)
	{
		glfwSwapInterval(vsync);
	}

	void Platform::SwapBuffers(Window* window)
	{
		glfwSwapBuffers(window->handler);
	}

	void Platform::MakeContextCurrent(Skore::Window* window)
	{
		glfwMakeContextCurrent(window->handler);
	}

	CPtr Platform::GetProcAddress()
	{
		return reinterpret_cast<CPtr>(glfwGetProcAddress);
	}

	void Platform::Shutdown()
	{
		glfwTerminate();
	}

	Window* Platform::CreateWindow(const StringView& title, UVec2 size, WindowFlags flags)
	{
		bool maximized = (flags & WindowFlags_Maximized);
		glfwWindowHint(GLFW_MAXIMIZED, maximized);

		float xScale = 1.f, yScale = 1.f;
		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xScale, &yScale);
		size = {u32(size.x * xScale), u32(size.y * yScale)};

		Window* window = new Window{};
		window->handler = glfwCreateWindow(size.x, size.y, title.Data(), nullptr, nullptr);

		PlatformStyle::ApplyDarkStyle(window->handler);

		glfwShowWindow(window->handler);

		return window;
	}

	Extent Platform::GetWindowExtent(Window* window)
	{
		i32 width, height;
		glfwGetWindowSize(window->handler, &width, &height);
		return Extent{static_cast<u32>(width), static_cast<u32>(height)};
	}

	void Platform::ProcessEvents()
	{
		glfwPollEvents();
	}

	bool Platform::UserRequestedClose(Window* window)
	{
		return glfwWindowShouldClose(window->handler);
	}

	void Platform::DestroyWindow(Window* window)
	{
		glfwDestroyWindow(window->handler);
		delete window;
	}

}

#endif