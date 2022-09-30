#pragma once

#include "Pixel/Renderer/Context.h"

struct GLFWwindow;

namespace Pixel {

	class OpenGLContext : public Context
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		virtual ~OpenGLContext();
		virtual void Initialize() override;
		virtual void SwapBuffers() override;
		virtual void Reset() override;
	private:
		GLFWwindow* m_WindowHandle;
	};
}
