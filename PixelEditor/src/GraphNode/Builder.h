#pragma once

#include "imgui-node-editor/imgui_node_editor.h"

namespace Pixel {
	namespace ed = ax::NodeEditor;

	class BlueprintNodeBuilder
	{
	public:
		BlueprintNodeBuilder(ImTextureID texture, int32_t textureWidth, int32_t textureHeight);

		//------node begin and node end------
		void Begin(ed::NodeId id);
		void End();
		//------node begin and node end------

		void Header(const ImVec4& color = ImVec4(1, 1, 1, 1));
		void EndHeader();

		void Input(ed::PinId id);
		void EndInput();

		void Output(ed::PinId id);
		void EndOutput();

		void Pin(ed::PinId id, ed::PinKind kind);
		void EndPin();

		void Middle();
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

		bool m_HasHeader;//have header, draw every node once, just clear this flag

		ed::NodeId m_CurrentNodeId;

		ImColor m_HeaderColor;//header color

		ImVec2 m_ContentMin;
		ImVec2 m_ContentMax;

		ImVec2 m_NodeMin;
		ImVec2 m_NodeMax;

		ImVec2 m_HeaderMin;
		ImVec2 m_HeaderMax;

		ImTextureID m_HeaderTextureId;

		int32_t m_HeaderTextureWidth;
		int32_t m_HeaderTextureHeight;

		bool SetStage(Stage newStage);
	};
}