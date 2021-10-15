#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Pixel/Events/ApplicationEvent.h"
#include "Window.h"

namespace Pixel {
	class PIXEL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		void OnEvent(Event& e);
	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	//To be defined in CLIENT
	Application* CreateApplication();
}


