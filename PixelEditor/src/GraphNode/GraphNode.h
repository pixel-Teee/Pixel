#pragma once

#include "imgui-node-editor/imgui_node_editor.h"

namespace Pixel {
	namespace ed = ax::NodeEditor;

	class ShaderFunction;
	class GraphPin;

	class GraphNode {
	public:
		ed::NodeId m_NodeId;

		Ref<ShaderFunction> p_Owner;
		
		//many input pin
		std::vector<Ref<GraphPin>> m_InputPin;

		//many output pins
		std::vector<Ref<GraphPin>> m_OutputPin;

		GraphNode() = default;
	};
}