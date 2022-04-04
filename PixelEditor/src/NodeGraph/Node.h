#pragma once

#include "Pixel/Renderer/3D/ShaderFunction.h"

#include <vector>
#include "imgui-node-editor/imgui_node_editor.h"

namespace Pixel {
	namespace ed = ax::NodeEditor;
	class GraphNode;
	class NodeLink;
	struct GraphPin {
		//pin id
		ed::PinId m_PinId;

		//pin belong's node
		Ref<GraphNode> m_Node;

		//current link, links the two pins
		Ref<NodeLink> m_NodeLink;

		//pin location index
		uint32_t m_LocationIndex;
		
		GraphPin() = default;
		friend bool operator==(GraphPin lhs, GraphPin rhs)
		{
			return lhs.m_PinId == rhs.m_PinId;
		}
	};

	class GraphNodeLink {
	public:
		ed::LinkId m_LinkId;
		
		//Input Pin
		Ref<GraphPin> m_InputPin;

		//Output Pin
		Ref<GraphPin> m_OutputPin;

		GraphNodeLink() = default;
	};

	class GraphNode {
	public:
		ed::NodeId m_NodeId;

		Ref<ShaderFunction> p_Owner;
		
		//one input pin
		std::vector<Ref<GraphPin>> m_InputPin;

		//many output pins
		std::vector<Ref<GraphPin>> m_OutputPin;

		GraphNode() = default;

		//------check have pin------
		bool HaveInputPin(GraphPin InputPin)
		{
			for (uint32_t i = 0; i < m_InputPin.size(); ++i)
			{
				if (*m_InputPin[i].get() == InputPin)
				{
					return true;
				}
			}
			return false;
		}

		bool HaveInputPin(ed::PinId pinId)
		{
			for (uint32_t i = 0; i < m_InputPin.size(); ++i)
			{
				if (m_InputPin[i]->m_PinId == pinId)
					return true;
			}
			return false;
		}

		bool HaveOutputPin(GraphPin OutputPin)
		{
			for (uint32_t i = 0; i < m_OutputPin.size(); ++i)
			{
				if (*m_OutputPin[i].get() == OutputPin)
				{
					return true;
				}
			}
			return false;
		}

		bool HaveOutputPin(ed::PinId nodeId)
		{
			for (uint32_t i = 0; i < m_OutputPin.size(); ++i)
			{
				if (m_OutputPin[i]->m_PinId == nodeId)
					return true;
			}
			return false;
		}
		//------check have pin------
	};
}