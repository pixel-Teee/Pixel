#include "EditorLayer.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Pixel/Scene/SceneSerializer.h"

#include "Pixel/Utils/PlatformUtils.h"
#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorGpuHandle.h"
#include "Pixel/Core/Application.h"
#include "Pixel/Renderer/Device/Device.h"

#include <chrono>

#include "ImGuizmo.h"

#include "Pixel/Math/Math.h"

namespace Pixel
{
	extern const std::filesystem::path g_AssetPath;

	EditorLayer::EditorLayer()
		:Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
	{

	}

	void EditorLayer::OnAttach()
	{
		//m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.dds");
		m_IconStop = Texture2D::Create("Resources/Icons/PauseButton.dds");

		m_IconPlayHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(1);
		m_IconStopHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(1);

		m_FrameBufferHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(1);

		Device::Get()->CopyDescriptorsSimple(1, m_IconPlayHandle->GetCpuHandle(), m_IconPlay->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_IconStopHandle->GetCpuHandle(), m_IconStop->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
		
		m_CameraController.SetZoomLevel(5.5f);
		/*------Create Geometry Framebuffer------*/

		/*------------------------------------
		R			G			B			A 
		position.r	position.g	position.b	x
		normal.r	normal.g	normal.b	x
		albedo.r	albedo.g	albedo.b	x
		roughness	metallic	emissive	x
		--------------------------------------*/
		FramebufferSpecification fbSpec;
		//fbSpec.Attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA8,
		//FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		//fbSpec.Width = 1280;
		//fbSpec.Height = 720;
		//m_GeoFramebuffer = Framebuffer::Create(fbSpec);
		///*------Create Geometry Framebuffer------*/

		///*------Create Framebuffer------*/	
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);
		Device::Get()->CopyDescriptorsSimple(1, m_FrameBufferHandle->GetCpuHandle(), m_Framebuffer->GetColorAttachmentDescriptorCpuHandle(0), DescriptorHeapType::CBV_UAV_SRV);
		///*------Create Framebuffer------*/

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		m_EditorCamera = EditorCamera(30.0f, 1.788f, 0.01f, 1000.0f);
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

		//Set Sky Box
		//ownership is belgon to Renderer3D
		//m_ActiveScene->SetSkyBox(Renderer3D::GetSkyBox());
		//m_EditorScene->SetSkyBox(Renderer3D::GetSkyBox());
		//m_environmentPanel.SetSkyBox(Renderer3D::GetSkyBox());

		//SceneSerializer serializer(m_ActiveScene);
		//serializer.Serialize("assets/scenes/Example.pixel");
		//serializer.Deserialize("assets/scenes/Example.pixel");
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

		//------Main Window Pos------//
		ImVec2 mainWindowPos = ImGui::GetWindowPos();
		//------Main Window Pos------//

		/*----------Render Panel----------*/
		m_SceneHierarchyPanel.OnImGuiRender();
		/*----------Render Panel----------*/

		/*----------Render Content Browser----------*/
		m_ContentBrowserPanel.OnImGuiRender();
		/*----------Render Content Browser----------*/

		/*----------Render Stats----------*/
		/*ImGui::Begin("Render Stats");

		std::string name = "None";
		if(m_HoveredEntity)
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
			ImGui::Text("Hovered Entity: %s", name.c_str());

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats");
		ImGui::Text("Draw Call: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::End();*/
		/*----------Render Stats----------*/

		/*----------View Port----------*/
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = {viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y};
		m_ViewportBounds[1] = {viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y};

		//PIXEL_CORE_INFO("{0}, {1}", viewportMinRegion.x, viewportMinRegion.y);

		//need to fix this:don't dependent this
		glm::vec2 ViewPortMidPoint = (m_ViewportBounds[0] + m_ViewportBounds[1]);
		ViewPortMidPoint.x /= 2;
		ViewPortMidPoint.y /= 2;

		Application::Get().GetWindow().SetViewPortCenterPoint(ViewPortMidPoint.x, ViewPortMidPoint.y);

		//PIXEL_CORE_INFO("{0}, {1}", ImGui::GetMousePos().x, ImGui::GetMousePos().y);

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();
		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};

		//uint64_t textureId = m_FrameBufferHandle->GetGpuPtr();
		Ref<DescriptorGpuHandle> pHandle = m_FrameBufferHandle->GetGpuHandle();
		ImGui::Image((ImTextureID)(pHandle->GetGpuPtr()), ImVec2{m_ViewportSize.x, m_ViewportSize.y});	

		//PIXEL_CORE_INFO("{0}, {1}", minBound.x, minBound.y);

		/*----------Drag Dop---------*/
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(std::filesystem::path(g_AssetPath) / path);
			}
			
			ImGui::EndDragDropTarget();
		}
		/*----------Drag Dop---------*/

		/*----------Gizmos----------*/
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();

			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			//camera entity
			//Runtime Camera
			//auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			//const auto& camera = cameraEntity.GetComponent<CameraComponent>().camera;
			//const glm::mat4& cameraProjection = camera.GetProjection();
			//glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());
			
			//Editor Camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			//entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();
			glm::vec3 originalRotation = tc.Rotation;

			//snap
			bool snap = Input::IsKeyPressed(PX_KEY_LEFT_CONTROL);
			float snapValue = 0.5f;
			//Snap to 45 dergees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
			{
				snapValue = 45.0f;
			}

			float snapValues[3] = {snapValue, snapValue, snapValue};

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), ImGuizmo::OPERATION(m_GizmoType),
			ImGuizmo::LOCAL, glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr);

			//glm::mat4 cubeMatrix = glm::mat4(1.0f);

			//ImGuizmo::DrawCubes(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), glm::value_ptr(cubeMatrix), 1);
			
			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 originRotation = tc.Rotation;
				glm::vec3 deltaRotation = rotation - originalRotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
		/*----------Gizmos----------*/

		if (ImGui::IsKeyPressed(KeyCodes::PX_KEY_ESCAPE))
		{
			Application::Get().GetWindow().SetCursorNormal();
		}

		/*----------View port----------*/

		/*---------Environment Panel---------*/
		//bool IsDirty = Renderer3D::GetSkyBox()->IsDirty();
		//if (IsDirty)
		//{	
		//	Renderer3D::GetSkyBox()->SetDirty(false);
		//	std::vector<std::string>& paths = Renderer3D::GetSkyBox()->GetPaths();

		//	for (uint32_t i = 0; i < 6; ++i)
		//	{
		//		if (paths[i] != m_environmentPanel.GetPath((FaceTarget)i))
		//		{
		//			m_environmentPanel.SetVisualizeFacesTexture((FaceTarget)i, paths[i]);
		//		}
		//	}
		//}
		//
		//m_environmentPanel.OnImGuiRender();
		/*---------Environment Panel---------*/

		/*---------Deferred Shading Viewport---------*/
		
	/*	ImGui::Begin("Deferred Shading Viewport");

		ImVec2 DeferredViewPortSize = ImVec2(m_ViewportSize.x / 4.0f, m_ViewportSize.y / 4.0f);

		ImGui::Text("position");
		uint32_t deferredTextureID = m_GeoFramebuffer->GetColorAttachmentRendererID(0);
		ImGui::Image(reinterpret_cast<void*>(deferredTextureID), ImVec2{ DeferredViewPortSize.x, DeferredViewPortSize.y }, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::Text("normal");
		deferredTextureID = m_GeoFramebuffer->GetColorAttachmentRendererID(1);
		ImGui::Image(reinterpret_cast<void*>(deferredTextureID), ImVec2{ DeferredViewPortSize.x, DeferredViewPortSize.y }, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::Text("albedo");
		deferredTextureID = m_GeoFramebuffer->GetColorAttachmentRendererID(2);
		ImGui::Image(reinterpret_cast<void*>(deferredTextureID), ImVec2{ DeferredViewPortSize.x, DeferredViewPortSize.y }, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::Text("depth");
		deferredTextureID = m_GeoFramebuffer->GetDepthAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(deferredTextureID), ImVec2{ DeferredViewPortSize.x, DeferredViewPortSize.y }, ImVec2(0, 1), ImVec2(1, 0));	
		ImGui::End();*/
		/*---------Deferred Shading Viewport---------*/

		/*---------Play Button And Pause Button---------*/
		UI_Toobar();
		/*---------Play Button And Pause Button----------*/
		ImGui::End();
		/*----------Dock Space----------*/
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		PX_PROFILE_FUNCTION();

		//Resize
		if(FramebufferSpecification spec = m_Framebuffer->GetSpecification();
		m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
		(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			//recopy to descriptor
			Device::Get()->CopyDescriptorsSimple(1, m_FrameBufferHandle->GetCpuHandle(), m_Framebuffer->GetColorAttachmentDescriptorCpuHandle(0), DescriptorHeapType::CBV_UAV_SRV);
			//m_GeoFramebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}
	
		//Render
		//Renderer2D::ResetStats();

		switch (m_SceneState)
		{
			case EditorLayer::SceneState::Edit:
			{
				if (m_ViewportFocused)
				{
					m_CameraController.OnUpdate(ts);
					m_EditorCamera.OnUpdate(ts);
				}
				//Update scene
				m_ActiveScene->OnUpdateEditorForward(ts, m_EditorCamera, m_Framebuffer);
				break;
			}
			case EditorLayer::SceneState::Play:
			{
				//m_ActiveScene->OnUpdateRuntime(ts, m_GeoFramebuffer, m_Framebuffer);
				break;
			}
		}

		//Calculate Mouse Pos in Viewport realtive pos
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;

		int mouseX = (int)mx;
		int mouseY = (int)my;
		//PIXEL_CORE_INFO("Mouse Pox = {0}, {1}", mouseX, mouseY);

		//m_GeoFramebuffer->Bind();
		//if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		//{
		//	int pixelData = m_GeoFramebuffer->ReadPixel(4, mouseX, mouseY);
		//	//PIXEL_CORE_INFO("PixelData = {0}", pixelData);
		//	m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		//}
		//m_GeoFramebuffer->UnBind();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(PX_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(PX_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
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
				if (control)
				{
					if(shift)
						SaveSceneAs();
					else
						SaveScene();
				}
					
				break;
			}

			case PX_KEY_D:
			{
				if(control)
					OnDuplicateEntity();
				break;
			}

			//Gizmos
			case PX_KEY_Q:
				m_GizmoType = -1;
				break;
			case PX_KEY_W:
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case PX_KEY_E:
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			case PX_KEY_R:
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
		}

		return true;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == PX_MOUSE_BUTTON_LEFT)
		{
			if(m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(PX_KEY_LEFT_ALT))
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	void EditorLayer::NewScene()
	{
		m_EditorScene = CreateRef<Scene>();
		//m_EditorScene->SetSkyBox(Renderer3D::GetDefaultSkyBox());
		m_ActiveScene = m_EditorScene;
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_CurrentScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		std::wstring filepath = FileDialogs::OpenFile(L"Pixel Scene (*.pixel)\0*.pixel\0");
		OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path filepath)
	{
		if(m_SceneState != SceneState::Edit)
			OnSceneStop();

		Ref<Scene> newScene = CreateRef<Scene>();
		glm::vec2 viewPortSize = m_EditorScene->GetViewPortSize();
		newScene->SetViewPortSize(viewPortSize.x, viewPortSize.y);
		//newScene->SetSkyBox(Renderer3D::GetSkyBox());
		SceneSerializer serializer(newScene);

		if (serializer.Deserialize(filepath.string()))
		{
			m_EditorScene = newScene;
			m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_CurrentScenePath = filepath;
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::wstring filepath = FileDialogs::SaveFile(L"Pixel Scene (*.pixel)\0*.pixel\0");

		if (!filepath.empty())
		{
			SerializerScene(m_ActiveScene, filepath);

			m_CurrentScenePath = filepath;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_CurrentScenePath.empty())
			SerializerScene(m_ActiveScene, m_CurrentScenePath);
		else
			SaveSceneAs();
	}

	void EditorLayer::SerializerScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	void EditorLayer::OnScenePlay()
	{
		//ImGui::CaptureMouseFromApp(true);
		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;
		m_ActiveScene->OnRuntimeStop();
		m_ActiveScene = m_EditorScene;
		//m_Editor->model entity's is dirty
		m_ActiveScene->MarkMeshEntityIDDirty();
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if(m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if(selectedEntity)
			m_EditorScene->DuplicateEntity(selectedEntity);
	}

	void EditorLayer::UI_Toobar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f;
		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.5f - size * 0.5f);

		Ref<DescriptorGpuHandle> pHandle = icon == m_IconPlay ? m_IconPlayHandle->GetGpuHandle() : m_IconStopHandle->GetGpuHandle();

		if (ImGui::ImageButton((ImTextureID)(pHandle->GetGpuPtr()), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (m_SceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}

}
