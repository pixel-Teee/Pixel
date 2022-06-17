#include "pxpch.h"
#include "Application.h"

#include "Pixel/Core/Log.h"

#include <glad/glad.h>
#include "Pixel/Renderer/Renderer.h"

#include "Input.h"

#include "glm/glm.hpp"

#include "GLFW/glfw3.h"

#if defined(_DEBUG)
#include <dxgi1_3.h>
#include <dxgidebug.h>
#endif

#ifndef PX_OPENGL
#include <wrl/client.h>
#include "Platform/DirectX/d3dx12.h"
#include "Platform/DirectX/DirectXRenderer.h"
#endif

namespace Pixel {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name)
	{
		PX_CORE_ASSERT(!s_Instance, "Application already exists!")
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name)));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		//Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer(m_Window->GetDevice());
		PushOverlay(m_ImGuiLayer);	

		m_pRenderer = BaseRenderer::Create();
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
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
		//PIXEL_TRACE("{0}", e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			if (e.Handled)
				break;
			(*--it)->OnEvent(e);			
		}
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
			float time = (float)glfwGetTime();//这是平台相关的获取Time的函数，未来会换掉
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
			
			auto[x, y] = Input::GetMousePosition();
			//PIXEL_CORE_TRACE("{0}, {1}", x, y);

			//Input::IsKeyPressed();

			m_Window->OnUpdate();
		}
	}
}
