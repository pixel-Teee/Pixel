#pragma once

#include "Pixel/Layer.h"

namespace Pixel {

	class PIXEL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		virtual void OnUpdate();
		virtual void OnEvent(Event& event);

		void OnAttach() override;
		void OnDetach() override;

	private:
		float m_Time = 0.0f;
	};
}
