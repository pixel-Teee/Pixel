#pragma once

#include "Pixel.h"

namespace Pixel {

	class GameLayer : public Pixel::Layer
	{
	public:
		GameLayer(){}
		~GameLayer(){}
		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(Timestep ts) override;
		//void OnImGuiRender() override;
		void OnEvent(Event& event) override;

		void OpenScene();
	private:
		bool OnWindowResizeEvent(WindowResizeEvent& e);
	private:
		Ref<Framebuffer> m_GeoFramebuffer;
		Ref<Framebuffer> m_Framebuffer;

		glm::vec2 m_ViewPortSize{1280, 720};

		Ref<Scene> m_GameScene;
	};
}
