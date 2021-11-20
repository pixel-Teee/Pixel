#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{

}

void Sandbox2D::OnAttach()
{
	
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

	Pixel::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Pixel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	Pixel::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.8f, 0.2f, 0.3f, 1.0f});
	Pixel::Renderer2D::EndScene();
	//glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
	//TODO: Shader::SetMat4, Shader::SetFloat4
	/*
	std::dynamic_pointer_cast<Pixel::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Pixel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);	

	Pixel::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
	*/	
}

void Sandbox2D::OnEvent(Pixel::Event& e)
{
	m_CameraController.OnEvent(e);
}
