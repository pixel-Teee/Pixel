#include "pxpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <Glad/glad.h>

namespace Pixel {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle):m_WindowHandle(windowHandle)
	{
		PX_CORE_ASSERT(windowHandle, "Window Handle is NULL");
	}

	OpenGLContext::~OpenGLContext()
	{

	}

	void OpenGLContext::Initialize()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		//glad¼ÓÔØOpenGLµÄº¯Êý
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		PX_CORE_ASSERT(status, "Failed to initialize Glad!");

		PIXEL_CORE_INFO("OpenGL Info:");
		PIXEL_CORE_INFO("Vendor : {0}", glGetString(GL_VENDOR));
		PIXEL_CORE_INFO("Renderer : {0}", glGetString(GL_RENDERER));
		PIXEL_CORE_INFO("Version : {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

	void OpenGLContext::Reset()
	{

	}

	void OpenGLContext::SetID(const std::wstring& ID)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::FlushResourceBarriers()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	uint64_t OpenGLContext::Flush(bool WaitForCompletion)
	{
		throw std::logic_error("The method or operation is not implemented.");
		return 0;
	}

	uint64_t OpenGLContext::Finish(bool WaitForCompletion)
	{
		throw std::logic_error("The method or operation is not implemented.");
		return 0;
	}

	void OpenGLContext::CopyBuffer(GpuResource& Dest, GpuResource& Src)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::CopyBufferRegion(GpuResource& Dest, size_t DestOffset, GpuResource& Src, size_t SrcOffset, size_t NumBytes)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::CopySubresource(GpuResource& Dest, uint32_t DestSubIndex, GpuResource& Src, uint32_t SrcSubIndex)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::InsertUAVBarrier(GpuResource& Resource, bool FlushImmediate)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::WriteBuffer(GpuResource& Dest, size_t DestOffset, const void* Data, size_t NumBytes)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}