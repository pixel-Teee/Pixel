#include "EditorLayer.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

namespace Pixel
{
	EditorLayer::EditorLayer()
		:Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
	{

	}

	void EditorLayer::OnAttach()
	{
		m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		m_CupTexture = Texture2D::Create("assets/textures/test.jpg");
		m_SpriteSheets = Texture2D::Create("assets/game/textures/tilemap_packed.png");

		m_TextureStairs = SubTexture2D::CreateFromCoords(m_SpriteSheets, { 14, 0 }, { 16, 16 }, { 1, 3 });

		m_CameraController.SetZoomLevel(5.5f);

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnImGuiRender()
	{
		 // If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
		// In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
		// In this specific demo, we are not using DockSpaceOverViewport() because:
		// - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
		// - we allow the host window to have padding (when opt_padding == true)
		// - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
		// TL;DR; this demo is more complicated than what you would normally use.
		// If we removed all the options we are showcasing, this demo would become:
		//     void ShowExampleAppDockSpace()
		//     {
		//         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		//     }
		static bool DockSpaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &DockSpaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGui::Begin("ColorEdit");
		ImGui::ColorEdit4("ColorTint", glm::value_ptr(m_SquareColor));
		ImGui::End();

		ImGui::Begin("Viewport");
		//PIXEL_WARN("Viewport is hoverd:{0}", ImGui::IsWindowHovered());
		//PIXEL_WARN("Viewport is focused:{0}", ImGui::IsWindowFocused());
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		PIXEL_WARN("Viewport is hoverd:{0}", ImGui::IsWindowHovered());
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if(m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
		{
			m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };		
			
			m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);
		}
		uint32_t textureId = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureId, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2(0, 1), ImVec2(1, 0));
		
		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		PX_PROFILE_FUNCTION();

		if(m_ViewportFocused)
			m_CameraController.OnUpdate(ts);

		//Timer timer("EditorLayer::OnUpdate", [&](ProfileResult profileResult){ m_ProfileResults.push_back(profileResult);});

		/*
		{
			PX_PROFILE_SCOPE("CameraController::OnUpdate");
			//Update
			m_CameraController.OnUpdate(ts);
		}
		*/
		//Render
		//Reset stats here
		Renderer2D::ResetStats();
		{
			PX_PROFILE_SCOPE("Renderer Prep");
			m_Framebuffer->Bind();
			//Render
			RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
			RenderCommand::Clear();
		}

#if 0
		{
			PX_PROFILE_SCOPE("Renderer Draw");

			static float rotation = 0.0f;
			rotation += ts * 20.0f;

			Renderer2D::BeginScene(m_CameraController.GetCamera());
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.8f, 0.2f, 0.3f, 1.0f });
			Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 1.0f, 1.0f }, m_CupTexture);
			Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.8f, 0.2f, 0.3f, 1.0f });
			Renderer2D::DrawQuad({ 0.5f, -0.5f, -0.3f }, { 2.0f, 2.0f }, m_CupTexture);



			//Renderer2D::DrawRotatedQuad({0.2f, 0.5f, -0.1f},{10.0f, 10.0f}, 45.0f, {2.0f, 1.0f, 0.8f, 1.0f});
			//Renderer2D::DrawQuad(glm::vec3(0.2f, 0.5f, -0.1f), glm::vec2(10.0f, 10.0f), m_CheckerboardTexture, 1.0f, glm::vec4(0.3f, 0.5f, 0.6f, 1.0f));
			//Renderer2D::DrawQuad(glm::vec3(-0.5f, -0.5f, 0.3f), glm::vec2(1.0f, 1.0f), m_CupTexture, 10.0f, glm::vec4(0.3f, 0.5f, 0.6f, 1.0f));
			//Renderer2D::DrawRotatedQuad({ 0.2f, 0.5f, -0.1f }, { 10.0f, 10.0f }, rotation, m_CheckerboardTexture, 10.0f);
			Renderer2D::EndScene();

			Renderer2D::BeginScene(m_CameraController.GetCamera());
			for (float y = -5.0f; y < 5.0f; y += 0.5f)
			{
				for (float x = -5.0f; x < 5.0f; x += 0.5f)
				{
					glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.5f };
					Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
				}
			}
			Renderer2D::EndScene();
		}
#endif
		Renderer2D::BeginScene(m_CameraController.GetCamera());
	
		Renderer2D::DrawQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureStairs);
		Renderer2D::DrawQuad({ -5.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_SquareColor);

		Renderer2D::EndScene();
		m_Framebuffer->UnBind();
		//glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
		//TODO: Shader::SetMat4, Shader::SetFloat4
		/*
		std::dynamic_pointer_cast<OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);

		Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		*/
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
	}

}
