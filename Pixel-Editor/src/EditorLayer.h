#pragma once

#include "Pixel.h"

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
		Pixel::OrthographicCameraController m_CameraController;

		Pixel::Ref<Pixel::VertexArray> m_SquareVA;
		Pixel::Ref<Pixel::Shader> m_FlatColorShader;

		Pixel::Ref<Pixel::Texture> m_CheckerboardTexture;
		Pixel::Ref<Pixel::Texture> m_CupTexture;

		//Sprite Sheets
		Pixel::Ref<Pixel::Texture2D> m_SpriteSheets;
		Pixel::Ref<Pixel::SubTexture2D> m_TextureStairs;

		Pixel::Ref<Pixel::Framebuffer> m_Framebuffer;

		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	};
}


