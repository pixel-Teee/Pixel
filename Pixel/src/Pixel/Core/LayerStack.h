#pragma once

#include "Pixel/Core/Core.h"
#include "Layer.h"

#include <vector>

namespace Pixel {
	class PIXEL_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin(){ return m_Layers.begin();}
		std::vector<Layer*>::iterator end(){ return m_Layers.end();}
		size_t size() { return m_Layers.size();  }
	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};
}


