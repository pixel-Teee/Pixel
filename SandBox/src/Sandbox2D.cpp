#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{

}

void Sandbox2D::OnAttach()
{
	//VertexArray
	m_SquareVA = Pixel::VertexArray::Create();

	//VertexBuffer
	float vertices2[5 * 4] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.5f,  0.5f, 0.0f,
	-0.5f,  0.5f, 0.0f
	};

	Pixel::Ref<Pixel::VertexBuffer> VertexBuffer2;
	VertexBuffer2.reset(Pixel::VertexBuffer::Create(vertices2, sizeof(vertices2)));
	VertexBuffer2->SetLayout({
		{Pixel::ShaderDataType::Float3,  "a_Position"}
	});
	m_SquareVA->AddVertexBuffer(VertexBuffer2);

	//IndexBuffer
	uint32_t indices2[6] = {
	0, 1, 2, 2, 3, 0
	};

	Pixel::Ref<Pixel::IndexBuffer> IndexBuffer2;
	IndexBuffer2.reset(Pixel::IndexBuffer::Create(indices2, sizeof(indices2) / sizeof(uint32_t)));

	m_SquareVA->SetIndexBuffer(IndexBuffer2);

	m_FlatColorShader = Pixel::Shader::Create("assets/shaders/FlatColor.glsl");
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("SquareColor", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnUpdate(Pixel::Timestep ts)
{
	//Update
	m_CameraController.OnUpdate(ts);

	//Render
	Pixel::RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
	Pixel::RenderCommand::Clear();

	Pixel::Renderer::BeginScene(m_CameraController.GetCamera());

	//glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

	std::dynamic_pointer_cast<Pixel::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Pixel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);	

	Pixel::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	Pixel::Renderer::EndScene();
}

void Sandbox2D::OnEvent(Pixel::Event& e)
{
	m_CameraController.OnEvent(e);
}
