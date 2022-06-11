#pragma once

#include "Pixel/Core/Window.h"
#include "Pixel/Renderer/Context/Context.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

namespace Pixel {
	class OpenGLContext;
	class DirectXDevice;
	class DirectXContext;
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth()const override{ return m_Data.Width;}
		inline unsigned int GetHeight()const override{ return m_Data.Height;}

		//Window Ù–‘
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback;}
		void SetVSync(bool enabled) override;
		bool IsVSync()const override;

		inline void* GetNativeWindow() const{ return m_Window; }

		virtual void SetCursorPos(int32_t x, int32_t y) override;
		virtual void SetCursorViewPortCenter() override;

		virtual void SetViewPortCenterPoint(int32_t x, int32_t y) override;
		virtual void SetCursorDisabled() override;
		virtual void SetCursorNormal() override;

		virtual Ref<Device> GetDevice() const override;

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		Ref<OpenGLContext> m_Context;
		Ref<Device> pDevice;

		//test:directx context
		//Context* m_DirectXContext;

		glm::vec2 m_ViewPortCenterPoint;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}


