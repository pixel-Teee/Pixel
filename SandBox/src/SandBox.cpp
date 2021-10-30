#include <Pixel.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Pixel::Layer
{
public:
	ExampleLayer():Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f), m_SquarePosition(0.0f)
	{
		//Vertex Array
		m_VertexArray.reset(Pixel::VertexArray::Create());

		//Vertex Buffer
		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f
		};
		m_VertexBuffer.reset(Pixel::VertexBuffer::Create(vertices, sizeof(vertices)));

		Pixel::BufferLayout layout = {
			{Pixel::ShaderDataType::Float3,  "a_Position"},
			{Pixel::ShaderDataType::Float4,  "a_Color"}
		};

		m_VertexBuffer->SetLayout(layout);

		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		//Index Buffer
		uint32_t indices[3] = { 0, 1, 2 };
		m_IndexBuffer.reset(Pixel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);
		//Shader

		std::string vertexSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
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

		m_Shader.reset(Pixel::Shader::Create(vertexSrc, fragmentSrc));

		//VertexArray
		m_VertexArray2.reset(Pixel::VertexArray::Create());

		//VertexBuffer
		float vertices2[4 * 5] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f, 0.5f, 0.0f,	1.0f, 1.0f,
		-0.5f, 0.5f, 0.0f,	0.0f, 1.0f	
		};

		Pixel::Ref<Pixel::VertexBuffer> VertexBuffer2;
		VertexBuffer2.reset(Pixel::VertexBuffer::Create(vertices2, sizeof(vertices2)));
		//IndexBuffer

		uint32_t indices2[6] = {
		0, 1, 2, 2, 3, 0
		};

		Pixel::BufferLayout layout2 = {
			{Pixel::ShaderDataType::Float3,  "a_Position"},
			{Pixel::ShaderDataType::Float2,  "a_TexCoord"}
		};

		Pixel::Ref<Pixel::IndexBuffer> IndexBuffer2;
		IndexBuffer2.reset(Pixel::IndexBuffer::Create(indices2, sizeof(indices2) / sizeof(uint32_t)));
		VertexBuffer2->SetLayout(layout2);

		m_VertexArray2->AddVertexBuffer(VertexBuffer2);
		m_VertexArray2->SetIndexBuffer(IndexBuffer2);

		//Shader
		std::string vertexSrc2 = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
			}
		)";

		std::string fragmentSrc2 = R"(
			#version 330 core
			out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				 color = vec4(u_Color, 1.0f);
			}
		)";

		m_Shader2.reset(Pixel::Shader::Create(vertexSrc2, fragmentSrc2));

		m_TextureShader.reset(Pixel::Shader::Create("assets/shaders/Texture.glsl"));

		//texture
		m_Texture = Pixel::Texture2D::Create("assets/textures/test2.png");

		std::dynamic_pointer_cast<Pixel::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<Pixel::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnUpdate(Pixel::Timestep ts)override
	{
		//PIXEL_TRACE("Delta time: {0}s ({1}ms)", ts.GetSeconds(), ts.GetMilliseconds());

		if(Pixel::Input::IsKeyPressed(PX_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		else if(Pixel::Input::IsKeyPressed(PX_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;

		if (Pixel::Input::IsKeyPressed(PX_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		else if (Pixel::Input::IsKeyPressed(PX_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;

		if (Pixel::Input::IsKeyPressed(PX_KEY_J))		
			m_SquarePosition.x -= m_SquareMoveSpeed * ts;
		else if (Pixel::Input::IsKeyPressed(PX_KEY_L))
			m_SquarePosition.x += m_SquareMoveSpeed * ts;

		if (Pixel::Input::IsKeyPressed(PX_KEY_I))
			m_SquarePosition.y += m_SquareMoveSpeed * ts;
		else if (Pixel::Input::IsKeyPressed(PX_KEY_K))
			m_SquarePosition.y -= m_SquareMoveSpeed * ts;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_SquarePosition);

		Pixel::RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
		Pixel::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(0.0f);

		Pixel::Renderer::BeginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		glm::vec4 redColor(0.7f, 0.2f, 0.2f, 1.0f);
		glm::vec4 blueColor(0.2f, 0.3f, 0.8f, 1.0f);

		std::dynamic_pointer_cast<Pixel::OpenGLShader>(m_Shader2)->Bind();
		std::dynamic_pointer_cast<Pixel::OpenGLShader>(m_Shader2)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; ++y)
		{
			for (int x = 0; x < 20; ++x)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Pixel::Renderer::Submit(m_Shader2, m_VertexArray2, transform);
			}
		}
	
		m_Texture->Bind();
		Pixel::Renderer::Submit(m_TextureShader, m_VertexArray2, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		//Triangle
		//Pixel::Renderer::Submit(m_Shader, m_VertexArray, transform);

		Pixel::Renderer::EndScene();
	}

	void OnEvent(Pixel::Event& event)override
	{
		Pixel::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<Pixel::KeyPressedEvent>(PX_BIND_EVENT_FN(ExampleLayer::OnPressed));
	}

	bool OnPressed(Pixel::KeyPressedEvent& event)
	{
		return false;
	}

private:
	Pixel::Ref<Pixel::Shader> m_Shader;
	Pixel::Ref<Pixel::VertexArray>  m_VertexArray;
	Pixel::Ref<Pixel::VertexBuffer> m_VertexBuffer;
	Pixel::Ref<Pixel::IndexBuffer>  m_IndexBuffer;

	Pixel::Ref<Pixel::Shader> m_Shader2, m_TextureShader;
	Pixel::Ref<Pixel::VertexArray> m_VertexArray2;

	Pixel::Ref<Pixel::Texture2D> m_Texture;

	Pixel::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 0.5f;

	glm::vec3 m_SquarePosition;
	float m_SquareMoveSpeed = 0.5f;

	glm::vec3 m_SquareColor = glm::vec3(1.0f);
};

class SandBox : public Pixel::Application
{
public:
	SandBox()
	{
		PushLayer(new ExampleLayer());
		//PushOverlay(new Pixel::ImGuiLayer());
	}

	~SandBox()
	{

	}
};

Pixel::Application* Pixel::CreateApplication()
{
	return new SandBox();
}