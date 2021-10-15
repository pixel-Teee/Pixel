#pragma once

#include "Pixel/Core.h"
#include "Pixel/Events/Event.h"

namespace Pixel
{
	class PIXEL_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach(){}
		virtual void OnDetach(){}
		virtual void OnUpdate(){}
		virtual void OnEvent(Event& event){}

		inline const std::string& GetName()const{ return m_DebugName;}
	protected:
		std::string m_DebugName;
	};
}
