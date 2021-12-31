#pragma once

#include "Pixel/Core/Layer.h"

#include "Pixel/Events/ApplicationEvent.h"
#include "Pixel/Events/KeyEvent.h"
#include "Pixel/Events/MouseEvent.h"

namespace Pixel {

	class PIXEL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e);
		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}
