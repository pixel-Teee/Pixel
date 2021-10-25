#include "pxpch.h"
#include "Application.h"

#include "Pixel/Log.h"

#include <glad/glad.h>
#include "Pixel/Renderer/Renderer.h"

#include "Input.h"

#include "glm/glm.hpp"

namespace Pixel {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application():m_Camera(-1.6f, 1.6f, -0.9f, 0.9f)
	{
		PX_CORE_ASSERT(!s_Instance, "Application already exists!")
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		//Vertex Array
		m_VertexArray.reset(VertexArray::Create());

		//Vertex Buffer
		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f
		};
		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		BufferLayout layout = {
			{ShaderDataType::Float3,  "a_Position"},
			{ShaderDataType::Float4,  "a_Color"}
		};

		m_VertexBuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		//Index Buffer
		uint32_t indices[3] = {0, 1, 2};
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
		//Shader

		std::string vertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = v_Color;
			}
		)";

		m_Shader.reset(new Shader(vertexSrc, fragmentSrc));

		//VertexArray
		m_VertexArray2.reset(VertexArray::Create());

		//VertexBuffer
		float vertices2[4 * 3] = {
		-0.75f, -0.75f, 0.0f,
		 0.75f, -0.75f, 0.0f,
		 0.75f, 0.75f, 0.0f,
		-0.75f, 0.75f, 0.0f
		};

		std::shared_ptr<VertexBuffer> VertexBuffer2;
		VertexBuffer2.reset(VertexBuffer::Create(vertices2, sizeof(vertices2)));
		//IndexBuffer

		uint32_t indices2[6] = {
		0, 1, 2, 2, 3, 0		
		};

		BufferLayout layout2 = {
			{ShaderDataType::Float3,  "a_Position"}
		};

		std::shared_ptr<IndexBuffer> IndexBuffer2;
		IndexBuffer2.reset(IndexBuffer::Create(indices2, sizeof(indices2) / sizeof(uint32_t)));
		VertexBuffer2->SetLayout(layout2);

		m_VertexArray2->AddVertexBuffer(VertexBuffer2);
		m_VertexArray2->SetIndexBuffer(IndexBuffer2);

		//Shader
		std::string vertexSrc2 = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0f);
			}
		)";

		std::string fragmentSrc2 = R"(
			#version 330 core
			out vec4 color;

			in vec3 v_Position;
			void main()
			{
				 color = vec4(0.3f, 0.2f, 0.4f, 1.0f);
			}
		)";

		m_Shader2.reset(new Shader(vertexSrc2, fragmentSrc2));
	}
	Application::~Application()
	{

	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		return true;
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		PIXEL_TRACE("{0}", e);

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if(e.Handled)
				break;
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
	
	void Application::Run()
	{
		while (m_Running)
		{
			RenderCommand::SetClearColor({0.1f, 0.2f, 0.3f, 1.0f});
			RenderCommand::Clear();

			Renderer::BeginScene(m_Camera);
			m_Camera.SetPosition({0.5f, 0.5f, 0.0f});
			m_Camera.SetRotation(45.0f);

			Renderer::Submit(m_Shader2, m_VertexArray2);
			Renderer::Submit(m_Shader, m_VertexArray);

			Renderer::EndScene();

			for(Layer* layer : m_LayerStack)
				layer->OnUpdate();

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
