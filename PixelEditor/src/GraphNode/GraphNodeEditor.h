#pragma once

#include "GraphNode.h"

namespace Pixel {
	namespace ed = ax::NodeEditor;

	class Material;
	class GraphLink;
	class GraphNode;
	class GraphPin;
	class GraphNodeEditor {
	public:

		GraphNodeEditor(const std::string& virtualPath, Ref<Material> pMaterial);

		~GraphNodeEditor();

		void OnImGuiRender(bool& OpenGraphNodeEditor);

	private:

		void DrawPbrNode();

		ed::EditorContext* m_Editor = nullptr;

		std::vector<Ref<GraphNode>> m_GraphNodes;
		std::vector<Ref<GraphPin>> m_GraphPins;
		std::vector<Ref<GraphLink>> m_GraphLinks;

		Ref<Material> m_pMaterial;
		
		uint32_t m_Id;
	};
}
