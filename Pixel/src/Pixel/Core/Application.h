#pragma once

#include "Core.h"

#include "Window.h"
#include "Pixel/Core/LayerStack.h"
#include "Pixel/Events/Event.h"
#include "Pixel/Events/ApplicationEvent.h"

#include "Pixel/Core/Timestep.h"

#include "Pixel/ImGui/ImGuiLayer.h"


namespace Pixel {
	class PIXEL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		
		void OnEvent(Event& e);

		//放置普通的层
		void PushLayer(Layer* layer);
		//放置Overlay在最后，UI层最后渲染，绘制在最上面
		void PushOverlay(Layer* layer);

		inline static Application& Get(){return *s_Instance;}
		inline Window& GetWindow(){ return *m_Window; }

		void Close();
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		Timestep m_Timestep;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	//To be defined in CLIENT
	Application* CreateApplication();
}


