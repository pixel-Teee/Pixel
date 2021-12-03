#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

//因为Lambda的类型是生成的，可以用模板来识别
template<typename Fn>
class Timer
{
public:
	Timer(const char* name, Fn&& func)
		:m_Name(name), m_Func(func), m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();
		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;

		float duration = (end - start) * 0.001f;

		std::cout << m_Name << "Duration: " << duration << "ms" << std::endl;

		m_Func({m_Name, duration});
	}

	~Timer()
	{
		if(!m_Stopped)
			Stop();
	}

private:
	const char* m_Name;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
	Fn m_Func;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult){m_ProfileResults.push_back(profileResult);})

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

	for (auto& result : m_ProfileResults)
	{
		char label[50];
		strcpy(label, result.Name);
		strcat(label, " %.3fms");
		ImGui::Text(label, result.Time);
	}
	//把m_ProfileResults到OnImGuiRender的都清空
	m_ProfileResults.clear();
	ImGui::End();
}

void Sandbox2D::OnUpdate(Pixel::Timestep ts)
{
	PROFILE_SCOPE("Sandbox2D::OnUpdate");

	//Timer timer("SandBox2D::OnUpdate", [&](ProfileResult profileResult){ m_ProfileResults.push_back(profileResult);});

	{
		PROFILE_SCOPE("CameraController::OnUpdate");
		//Update
		m_CameraController.OnUpdate(ts);
	}
	
	{
		PROFILE_SCOPE("Renderer Prep");
		//Render
		Pixel::RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
		Pixel::RenderCommand::Clear();
	}

	{
		PROFILE_SCOPE("Renderer Draw");
		Pixel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Pixel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Pixel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Pixel::Renderer2D::DrawQuad({ 0.2f, 0.5f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture);
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
