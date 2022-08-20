#pragma once

#include "GraphNode.h"

namespace Pixel {
	namespace ed = ax::NodeEditor;

	class GraphNodeEditor {
	public:
		GraphNodeEditor();

		~GraphNodeEditor();

		void OnImGuiRender();

	private:

		void DrawPbrNode();
	};
}
