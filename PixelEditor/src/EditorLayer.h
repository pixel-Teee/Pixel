#pragma once

#include "Pixel.h"
#include "Panels/SceneHierarchyPanel.h"

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
		OrthographicCameraController m_CameraController;

		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;

		Ref<Texture> m_CheckerboardTexture;
		Ref<Texture> m_CupTexture;

		//Sprite Sheets
		Ref<Texture2D> m_SpriteSheets;
		Ref<SubTexture2D> m_TextureStairs;

		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		Entity m_square;
		Entity m_RedSquare;
		Entity m_CameraEntity;
		Entity m_CameraEntity2;
		bool PrimiaryCamera = true;
		//Viewport Size
		glm::vec2 m_ViewportSize = {0, 0};
		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		//Panesl
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};
}


