#include "pxpch.h"

//------others library------
#include <glad/glad.h>
#include "glm/glm.hpp"
#include "GLFW/glfw3.h"
//------others library------

//------my library------
#include "Pixel/Core/Application.h"
#include "Pixel/Core/Input.h"
#include "Pixel/Core/Log.h"
#include "Pixel/Renderer/BaseRenderer.h"
#include "Pixel/Renderer/Device/Device.h"
//------my library------

namespace Pixel {

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name)
	{
		PX_CORE_ASSERT(!s_Instance, "application already exists!")

		s_Instance = this;
		//------create window------
		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name)));
		m_Window->SetEventCallback(PX_BIND_EVENT_FN(Application::OnEvent));
		//------create window------

		//------create imgui layer------
		m_ImGuiLayer = new ImGuiLayer(m_Window->GetDevice());
		PushOverlay(m_ImGuiLayer);	
		//------create imgui layer------

		//------create renderer------
		m_pRenderer = BaseRenderer::Create();
		m_pRenderer->Initialize();
		//------create renderer------
	}

	Application::~Application()
	{

	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		//------destory imgui layer's swap chain, because imgui layer holds the swap chain buffer------
		m_ImGuiLayer->ResetSwapChain();
		//------destroy imgui layer's swap chain, because imgui layer holds the swap chain buffer------
		
		Device::Get()->SetClientSize(e.GetWidth(), e.GetHeight());

		//recreate swap chain
		Device::Get()->ReCreateSwapChain();

		m_ImGuiLayer->ReCreateSwapChain();

		return false;
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(PX_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(PX_BIND_EVENT_FN(Application::OnWindowResize));

		//------dispatch all layer------
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			if (e.Handled)
				break;
			(*--it)->OnEvent(e);			
		}
		//------dispatch all layer------
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}
	
	void Application::Close()
	{
		m_Running = false;
	}

	Ref<BaseRenderer> Application::GetRenderer()
	{
		return m_pRenderer;
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime();//this platform-depend function, will get rid in the future

			Timestep timestep = time - m_LastFrameTime;

			m_LastFrameTime = time;
			
			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}
			
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}
}
