#pragma once

#include "Pixel/Renderer/3D/Model.h"
#include "imgui-node-editor/imgui_node_editor.h"

namespace Pixel {
	class NodeGraph {
	public:
		NodeGraph();

		void OnImGuiRender();
	private:
		Model model;
	};
}
