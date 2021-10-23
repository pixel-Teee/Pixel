#include "pxpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <Glad/glad.h>

namespace Pixel {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle):m_WindowHandle(windowHandle)
	{
		PX_CORE_ASSERT(windowHandle, "Window Handle is NULL");
	}

	void OpenGLContext::Init()
	{
		//^
		glfwMakeContextCurrent(m_WindowHandle);
		//glad¼ÓÔØOpenGLµÄº¯Êý
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		PX_CORE_ASSERT(status, "Failed to initialize Glad!");
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}