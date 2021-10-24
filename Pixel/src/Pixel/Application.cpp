#include "pxpch.h"
#include "Application.h"

#include "Pixel/Log.h"

#include <glad/glad.h>

#include "Input.h"

#include "glm/glm.hpp"

namespace Pixel {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	static GLenum ShaderDataTypeToOpenGLBasetype(ShaderDataType type)
	{
		switch (type)
		{
			case Pixel::ShaderDataType::None:	return GL_FLOAT;
			case Pixel::ShaderDataType::Float:	return GL_FLOAT;
			case Pixel::ShaderDataType::Float2:	return GL_FLOAT;
			case Pixel::ShaderDataType::Float3:	return GL_FLOAT;
			case Pixel::ShaderDataType::Float4:	return GL_FLOAT;
			case Pixel::ShaderDataType::Mat3:	return GL_FLOAT;
			case Pixel::ShaderDataType::Mat4:	return GL_FLOAT;
			case Pixel::ShaderDataType::Int:	return GL_INT;
			case Pixel::ShaderDataType::Int2:	return GL_INT;
			case Pixel::ShaderDataType::Int3:	return GL_INT;
			case Pixel::ShaderDataType::Int4:	return GL_INT;
			case Pixel::ShaderDataType::Bool:	return GL_BOOL;
		}
		PX_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	Application::Application()
	{
		PX_CORE_ASSERT(!s_Instance, "Application already exists!")
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		//Vertex Array
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f
		};

		BufferLayout layout = {
			{ShaderDataType::Float3,  "a_Position"},
			{ShaderDataType::Float4,  "a_Color"}
		};

		//Vertex Buffer
		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
		m_VertexBuffer->SetLayout(layout);

		uint32_t index = 0;
		for (const auto& element : m_VertexBuffer->GetLayout())
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, 
			element.GetComponentCount(), 
			ShaderDataTypeToOpenGLBasetype(element.Type),
			element.Normalized? GL_TRUE : GL_FALSE,
			layout.GetStride(),
			(const void*)element.Offset);
			++index;
		}

		//Index Buffer
		uint32_t indices[3] = {0, 1, 2};
		m_IndexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		//Shader

		std::string vertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0f);
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
			glClearColor(0.2f, 0.2f, 0.2f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			m_Shader->Bind();
			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

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
