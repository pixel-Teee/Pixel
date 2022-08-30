#pragma once

#include "imgui-node-editor/imgui_node_editor.h"

namespace Pixel {
	namespace ed = ax::NodeEditor;

	class GraphLink;

	class GraphNode;

	class GraphPin {
	public:
		//pin id
		ed::PinId m_PinId;

		ImColor m_Color;

		//pin belong's node
		std::weak_ptr<GraphNode> m_OwnerNode;

		//pin name
		std::string m_PinName;

		//current link, links the two pins
		Ref<GraphLink> m_NodeLink;

		//pin location index

		GraphPin();
		friend bool operator==(const GraphPin& lhs, const GraphPin& rhs);
	};
}