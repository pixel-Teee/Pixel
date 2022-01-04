#include "EditorLayer.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Pixel/Scene/SceneSerializer.h"

#include "Pixel/Utils/PlatformUtils.h"

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

		m_ActiveScene = CreateRef<Scene>();
#if 0	
		//entity
		m_square = m_ActiveScene->CreateEntity("Square");
		m_square.AddComponent<SpriteRendererComponent>(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));	

		//Red Square
		m_RedSquare = m_ActiveScene->CreateEntity("Red Square");
		m_RedSquare.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera");
		m_CameraEntity.AddComponent<CameraComponent>().Primary = false;
	
		m_CameraEntity2 = m_ActiveScene->CreateEntity("Camera2");
		m_CameraEntity2.AddComponent<CameraComponent>();

		class CameraController : public ScriptableEntity
		{
		public:
			void OnCreate()
			{
				//std::cout << "OnCreate" << std::endl;
				auto& translation = GetComponent<TransformComponent>().Translation;
				translation.x = rand() % 10 - 5.0f;
			}

			void OnDestroy()
			{

			}

			void OnUpdate(Timestep ts)
			{
				//std::cout << "Timestep: " << ts << std::endl;

				auto& translation = GetComponent<TransformComponent>().Translation;
				float speed = 5.0f;

				if(Input::IsKeyPressed(PX_KEY_A))
					translation.x -= speed * ts;
				if (Input::IsKeyPressed(PX_KEY_D))
					translation.x += speed * ts;
				if (Input::IsKeyPressed(PX_KEY_W))
					translation.y += speed * ts;
				if (Input::IsKeyPressed(PX_KEY_S))
					translation.y -= speed * ts;
			}
		};
		m_CameraEntity2.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		SceneSerializer serializer(m_ActiveScene);
		//serializer.Serialize("assets/scenes/Example.pixel");
		serializer.Deserialize("assets/scenes/Example.pixel");
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
		/*----------Dock Space----------*/
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
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
				{
					NewScene();
				}

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
				{
					OpenScene();
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
				{
					SaveSceneAs();
				}

				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		/*----------Render Panel----------*/
		m_SceneHierarchyPanel.OnImGuiRender();
		/*----------Render Panel----------*/

		/*----------Render Stats----------*/
		ImGui::Begin("Render Stats");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats");
		ImGui::Text("Draw Call: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::End();
		/*----------Render Stats----------*/

		/*----------View Port----------*/
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");
		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if(m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
		{
			m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };		
			
			m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);

			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}
		uint32_t textureId = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)textureId, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2(0, 1), ImVec2(1, 0));	
		ImGui::End();
		ImGui::PopStyleVar();
		/*----------View port----------*/
		ImGui::End();
		/*----------Dock Space----------*/
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		PX_PROFILE_FUNCTION();

		if(m_ViewportFocused)
			m_CameraController.OnUpdate(ts);
		//Render
		Renderer2D::ResetStats();
		//PX_PROFILE_SCOPE("Renderer Prep");
		m_Framebuffer->Bind();
		
		RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
		RenderCommand::Clear();
		
		//Update scene
		m_ActiveScene->OnUpdate(ts);

		m_Framebuffer->UnBind();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(PX_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		//Shortcuts
		if(e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(PX_KEY_LEFT_CONTROL) || Input::IsKeyPressed(PX_KEY_RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(PX_KEY_LEFT_SHIFT) || Input::IsKeyPressed(PX_KEY_RIGHT_SHIFT);
		switch (e.GetKeyCode())
		{
			case PX_KEY_N:
			{
				if (control)
					NewScene();
				break;
			}
			case PX_KEY_O:
			{
				if (control)
					OpenScene();
				break;
			}
			case PX_KEY_S:
			{
				if (control && shift)
					SaveSceneAs();
				break;
			}
		}
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Pixel Scene (*.pixel)\0*.pixel\0");
		if (!filepath.empty())
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_ActiveScene);

			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Pixel Scene (*.pixel)\0*.pixel\0");

		SceneSerializer serializer(m_ActiveScene);
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);
		}
	}

}
