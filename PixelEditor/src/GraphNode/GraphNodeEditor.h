#pragma once

#include "GraphNode.h"

namespace Pixel {
	namespace ed = ax::NodeEditor;

	class GraphNodeEditor {
	public:

		GraphNodeEditor();

		~GraphNodeEditor();

		void OnImGuiRender(bool& OpenGraphNodeEditor);

	private:

		void DrawPbrNode();

		ed::EditorContext* m_Editor = nullptr;

		bool m_FirstOpen;
	};
}
