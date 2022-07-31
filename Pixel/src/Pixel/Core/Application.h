#pragma once

//-----others library------
#include "Pixel/ImGui/ImGuiLayer.h"
//-----others library------

//------my library------
#include "Pixel/Core/Core.h"
#include "Pixel/Core/Window.h"
#include "Pixel/Core/Timestep.h"
#include "Pixel/Core/LayerStack.h"

#include "Pixel/Events/Event.h"
#include "Pixel/Events/ApplicationEvent.h"
//------my library------

namespace Pixel {

	class BaseRenderer;

	class PIXEL_API Application
	{
	public:
		Application(const std::string& name = "Pixel Engine");

		virtual ~Application();

		void Run();
		
		void OnEvent(Event& e);

		//place normal layer
		void PushLayer(Layer* layer);
		//place overlay in the endest
		void PushOverlay(Layer* layer);

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		Ref<BaseRenderer> GetRenderer();

		inline static Application& Get(){ return *s_Instance;}

		inline Window& GetWindow(){ return *m_Window; }

		void Close();

		Layer* GetLayer(int32_t layerIndex)
		{
			PX_CORE_ASSERT(layerIndex < m_LayerStack.size(), "Layer Index Out Of Bound");

			std::vector<Layer*>::iterator it = m_LayerStack.begin();

			return *(it + layerIndex);
		}

	private:

		bool OnWindowClose(WindowCloseEvent& e);

		bool OnWindowResize(WindowResizeEvent& e);

	private:

		Ref<BaseRenderer> m_pRenderer;

		Scope<Window> m_Window;

		ImGuiLayer* m_ImGuiLayer;

		bool m_Running = true;

		bool m_Minimized = false;

		LayerStack m_LayerStack;

		Timestep m_Timestep;

		float m_LastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
	};

	//to be defined in client
	Application* CreateApplication();
}


