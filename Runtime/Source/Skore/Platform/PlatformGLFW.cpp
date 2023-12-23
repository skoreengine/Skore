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

	struct GLFWWindowHandler
	{
		GLFWwindow* GlfwWindow;
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
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	}

	void Platform::SetVSync(bool vsync)
	{
		glfwSwapInterval(vsync);
	}

	void Platform::SwapBuffers(Window window)
	{
		glfwSwapBuffers(static_cast<GLFWWindowHandler*>(window.Handler)->GlfwWindow);
	}

	void Platform::MakeContextCurrent(Window window)
	{
		glfwMakeContextCurrent(static_cast<GLFWWindowHandler*>(window.Handler)->GlfwWindow);
	}

	CPtr Platform::GetProcAddress()
	{
		return reinterpret_cast<CPtr>(glfwGetProcAddress);
	}

	void Platform::Shutdown()
	{
		glfwTerminate();
	}

	Window Platform::CreateWindow(const StringView& title, UVec2 size, WindowFlags flags)
	{
		bool maximized = (flags & WindowFlags_Maximized);
		glfwWindowHint(GLFW_MAXIMIZED, maximized);

		float xScale = 1.f, yScale = 1.f;
		glfwGetMonitorContentScale(glfwGetPrimaryMonitor(), &xScale, &yScale);
		size = {u32(size.X * xScale), u32(size.Y * yScale)};


		GLFWWindowHandler* handler = new GLFWWindowHandler{};
		handler->GlfwWindow = glfwCreateWindow(size.X, size.Y, title.Data(), nullptr, nullptr);

		PlatformStyle::ApplyDarkStyle(handler->GlfwWindow);
		glfwShowWindow(handler->GlfwWindow);

		return {handler};
	}

	Extent Platform::GetWindowExtent(Window window)
	{
		i32 width, height;
		glfwGetWindowSize(static_cast<GLFWWindowHandler*>(window.Handler)->GlfwWindow, &width, &height);
		return Extent{static_cast<u32>(width), static_cast<u32>(height)};
	}

	void Platform::ProcessEvents()
	{
		glfwPollEvents();
	}

	bool Platform::UserRequestedClose(Window window)
	{
		return glfwWindowShouldClose(static_cast<GLFWWindowHandler*>(window.Handler)->GlfwWindow);
	}

	void Platform::DestroyWindow(Window window)
	{
		glfwDestroyWindow(static_cast<GLFWWindowHandler*>(window.Handler)->GlfwWindow);
		delete static_cast<GLFWWindowHandler*>(window.Handler);
	}

}

#endif