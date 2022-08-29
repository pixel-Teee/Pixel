#pragma once

#include "imgui-node-editor/imgui_node_editor.h"

namespace Pixel {
	namespace ed = ax::NodeEditor;

	class BlueprintNodeBuilder
	{
	public:
		BlueprintNodeBuilder();

		//------node begin and node end------
		void Begin(ed::NodeId id);
		void End();
		//------node begin and node end------
	private:
		enum class Stage
		{
			Invalid,
			Begin,
			Header,
			Content,
			Input,
			Output,
			Middle,
			End
		};

		Stage m_CurrentStage;
	};
}