#pragma once

#include "imgui-node-editor/imgui_node_editor.h"

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

		void DrawMainFunctionNode();

		ed::EditorContext* m_Editor = nullptr;

		std::string m_GraphNodeEditorConfigPath;

		std::vector<Ref<GraphNode>> m_GraphNodes;
		std::vector<Ref<GraphPin>> m_GraphPins;
		std::vector<Ref<GraphLink>> m_GraphLinks;

		Ref<Material> m_pMaterial;
		
		uint32_t m_Id;
	};
}
