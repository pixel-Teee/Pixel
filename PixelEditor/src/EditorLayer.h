#pragma once

#include "Pixel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/EnvironmentPanel.h"
#include "NodeGraph/NodeGraph.h"
#include "Pixel/Renderer/EditorCamera.h"

namespace Pixel
{
	class EditorLayer : public Pixel::Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		void OnImGuiRender() override;
		virtual void OnUpdate(Timestep ts)override;
		void OnEvent(Event& e)override;
	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		void NewScene();
		void SaveScene();
		void OpenScene();
		//------test------
		void SaveSceneAs();
		void SerializerScene(Ref<Scene> scene, const std::filesystem::path& path);
		void OpenSceneAs();
		void OpenScene(const std::filesystem::path filepath);
		//------test------

		void OnScenePlay();
		void OnSceneStop();

		void OnDuplicateEntity();

		//UI Panels
		void UI_Toobar();
	private:
		OrthographicCameraController m_CameraController;

		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;

		Ref<Texture> m_CheckerboardTexture;
		Ref<Texture> m_CupTexture;

		//Sprite Sheets
		Ref<Texture2D> m_SpriteSheets;
		Ref<SubTexture2D> m_TextureStairs;

		Ref<Framebuffer> m_GeoFramebuffer;

		Ref<Framebuffer> m_Framebuffer;

		Ref<Framebuffer> m_FinalFrameBuffer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_CurrentScenePath;

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		Entity m_square;
		Entity m_RedSquare;
		Entity m_CameraEntity;
		Entity m_CameraEntity2;
		bool PrimiaryCamera = true;
		EditorCamera m_EditorCamera;

		//Viewport Size
		glm::vec2 m_ViewportSize = {0, 0};
		//Viewport Coordinate
		glm::vec2 m_ViewportBounds[2];
		//Hovered Entity
		Entity m_HoveredEntity;

		//GizmoType
		int m_GizmoType = -1;

		//Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		EnvironmentPanel m_environmentPanel;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};

		SceneState m_SceneState = SceneState::Edit;

		//Editor Resources
		Ref<Texture2D> m_IconPlay, m_IconStop;

		Ref<DescriptorHandle> m_IconPlayHandle, m_IconStopHandle;

		//debug
		Ref<DescriptorHandle> m_ShadowMapHandle;

		Ref<DescriptorHandle> m_FrameBufferHandle;

		Ref<DescriptorHandle> m_UVBufferHandle;
		//----------test
		Ref<Shader> m_testShader;
		Model m_Model;
		//----------test
	};
}


