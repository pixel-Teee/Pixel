#pragma once

#include "Pixel/Renderer/3D/Model.h"
#include "Pixel/Renderer/3D/MaterialPass.h"
#include "imgui-node-editor/imgui_node_editor.h"

namespace Pixel {
	namespace ed = ax::NodeEditor;

	class NodeGraph {
	public:
		NodeGraph();
		~NodeGraph();
		void OnImGuiRender();
	private:
		Model m_previewBox;

		//TODO:move to scene
		MaterialPass m_materialPass;

		Model m_model;

		Ref<Camera> m_pCamera;

		Ref<StaticMesh> m_pStaticMesh;

		Ref<Material> m_pMaterial;
		Ref<MaterialInstance> m_pMaterialInstance;

		bool m_isopen;
		bool m_isFirstFrame;

		int m_uniqueId;
		ed::EditorContext* g_Context = nullptr;
	};
}
