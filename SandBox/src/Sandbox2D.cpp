#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{

}

void Sandbox2D::OnAttach()
{
	m_CheckerboardTexture = Pixel::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("SquareColor", glm::value_ptr(m_SquareColor));

	/*
	for (auto& result : m_ProfileResults)
	{
		char label[50];
		strcpy(label, result.Name);
		strcat(label, " %.3fms");
		ImGui::Text(label, result.Time);
	}
	//把m_ProfileResults到OnImGuiRender的都清空
	m_ProfileResults.clear();
	*/
	ImGui::End();
}

void Sandbox2D::OnUpdate(Pixel::Timestep ts)
{
	PX_PROFILE_FUNCTION();

	//Timer timer("SandBox2D::OnUpdate", [&](ProfileResult profileResult){ m_ProfileResults.push_back(profileResult);});

	{
		PX_PROFILE_SCOPE("CameraController::OnUpdate");
		//Update
		m_CameraController.OnUpdate(ts);
	}
	
	{
		PX_PROFILE_SCOPE("Renderer Prep");
		//Render
		Pixel::RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
		Pixel::RenderCommand::Clear();
	}

	{
		PX_PROFILE_SCOPE("Renderer Draw");
		Pixel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Pixel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Pixel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		//Pixel::Renderer2D::DrawRotatedQuad({0.2f, 0.5f, -0.1f},{10.0f, 10.0f}, 45.0f, {2.0f, 1.0f, 0.8f, 1.0f});
		Pixel::Renderer2D::DrawRotatedQuad(glm::vec3(0.2f, 0.5f, -0.1f), glm::vec2(10.0f, 10.0f), 30.0f, m_CheckerboardTexture, 10.0f);
		//Pixel::Renderer2D::DrawRotatedQuad({ 0.2f, 0.5f, -0.1f }, { 10.0f, 10.0f }, glm::radians(30.0f), m_CheckerboardTexture, 10.0f);
		Pixel::Renderer2D::EndScene();
	}
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
