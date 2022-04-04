#pragma once

#include "Pixel/Renderer/3D/Model.h"
#include "Pixel/Renderer/3D/MaterialPass.h"
#include "Pixel/Renderer/3D/GeometryPass.h"
#include "imgui-node-editor/imgui_node_editor.h"

#include "Node.h"

namespace Pixel {
	namespace ed = ax::NodeEditor;

	class NodeGraph {
	public:
		NodeGraph();
		~NodeGraph();
		void OnImGuiRender();
	private:
		void DrawPbrNode();
		bool CreateNewNodeMenu();

		void CreateNodePin(Ref<GraphNode> graphNode, uint32_t locationId);

		Model m_previewBox;

		//TODO:move to scene class
		GeometryPass m_materialPass;

		Ref<Model> m_Model;
		Ref<Camera> m_pCamera;
		Ref<StaticMesh> m_pStaticMesh;
		Ref<Material> m_pMaterial;
		Ref<MaterialInstance> m_pMaterialInstance;

		bool m_isopen;
		bool m_isFirstFrame;

		//------Graph Editor------
		int32_t m_uniqueId;
		ed::EditorContext* g_Context = nullptr;
		Ref<GraphNode> m_pbrNode;
		std::vector<Ref<GraphNodeLink>> m_NodeLink;
		std::vector<Ref<GraphNode>> m_Nodes;
		//------Graph Editor------
		glm::vec2 m_MousePos;
	};
}
