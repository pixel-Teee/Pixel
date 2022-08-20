#pragma once

#include "imgui-node-editor/imgui_node_editor.h"

namespace Pixel {
	namespace ed = ax::NodeEditor;

	class GraphPin;

	class GraphLink {
	public:
		ed::LinkId m_LinkId;

		//input pin
		std::weak_ptr<GraphPin> m_InputPin;

		std::weak_ptr<GraphPin> m_OutputPin;

		friend bool operator==(const GraphLink& lhs, const GraphLink& rhs);

		friend bool operator!=(const GraphLink& lhs, const GraphLink& rhs);
	};
}