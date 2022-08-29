#include "pxpch.h"

#include "Builder.h"

namespace Pixel {
	BlueprintNodeBuilder::BlueprintNodeBuilder()
	{
	}
	void BlueprintNodeBuilder::Begin(ed::NodeId id)
	{
		ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));

		ed::BeginNode(id);

		//SetStage(Stage::Begin);
	}
	void BlueprintNodeBuilder::End()
	{
		ed::EndNode();

		if (ImGui::IsItemVisible())
		{

		}
	}
}