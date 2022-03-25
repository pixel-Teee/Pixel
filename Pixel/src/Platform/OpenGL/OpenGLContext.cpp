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
		glfwMakeContextCurrent(m_WindowHandle);
		//glad����OpenGL�ĺ���
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		PX_CORE_ASSERT(status, "Failed to initialize Glad!");

		PIXEL_CORE_INFO("OpenGL Info:");
		PIXEL_CORE_INFO("Vendor : {0}", glGetString(GL_VENDOR));
		PIXEL_CORE_INFO("Renderer : {0}", glGetString(GL_RENDERER));
		PIXEL_CORE_INFO("Version : {0}", glGetString(GL_VERSION));

		if (GL_SPIR_V_EXTENSIONS)
		{
			PIXEL_CORE_INFO("Support spirv module!");
		}
		else
		{
			PIXEL_CORE_INFO("Don't support spirv module!");
		}
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}