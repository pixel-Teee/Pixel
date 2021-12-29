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
	m_CupTexture = Pixel::Texture2D::Create("assets/textures/test.jpg");
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnImGuiRender()
{
	//ImGui::Begin("Settings");
	//ImGui::ColorEdit4("SquareColor", glm::value_ptr(m_SquareColor));

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
	//ImGui::End();
	ImGui::Begin("Settings");

	auto stats = Pixel::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("SquareColor", glm::value_ptr(m_SquareColor));
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
	
	//Render
	//Reset stats here
	Pixel::Renderer2D::ResetStats();
	{
		PX_PROFILE_SCOPE("Renderer Prep");
		//Render
		Pixel::RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
		Pixel::RenderCommand::Clear();
	}

	{
		PX_PROFILE_SCOPE("Renderer Draw");

		static float rotation = 0.0f;
		rotation += ts * 20.0f;

		Pixel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Pixel::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Pixel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 1.0f, 1.0f }, m_CupTexture);
		Pixel::Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Pixel::Renderer2D::DrawQuad({ 0.5f, -0.5f, -0.3f }, { 2.0f, 2.0f }, m_CupTexture);
		
		
		
		//Pixel::Renderer2D::DrawRotatedQuad({0.2f, 0.5f, -0.1f},{10.0f, 10.0f}, 45.0f, {2.0f, 1.0f, 0.8f, 1.0f});
		//Pixel::Renderer2D::DrawQuad(glm::vec3(0.2f, 0.5f, -0.1f), glm::vec2(10.0f, 10.0f), m_CheckerboardTexture, 1.0f, glm::vec4(0.3f, 0.5f, 0.6f, 1.0f));
		//Pixel::Renderer2D::DrawQuad(glm::vec3(-0.5f, -0.5f, 0.3f), glm::vec2(1.0f, 1.0f), m_CupTexture, 10.0f, glm::vec4(0.3f, 0.5f, 0.6f, 1.0f));
		//Pixel::Renderer2D::DrawRotatedQuad({ 0.2f, 0.5f, -0.1f }, { 10.0f, 10.0f }, rotation, m_CheckerboardTexture, 10.0f);
		Pixel::Renderer2D::EndScene();

		Pixel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = {(x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.5f};
				Pixel::Renderer2D::DrawQuad({x, y}, {0.45f, 0.45f}, color);
			}
		}
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
