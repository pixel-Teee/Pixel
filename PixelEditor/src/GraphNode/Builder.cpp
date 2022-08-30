#include "pxpch.h"

#include "Builder.h"

//------other library------
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
//------other library------

namespace Pixel {
	BlueprintNodeBuilder::BlueprintNodeBuilder(ImTextureID texture, int32_t textureWidth, int32_t textureHeight)
	:m_HeaderTextureId(texture),
	m_HeaderTextureWidth(textureWidth),
	m_HeaderTextureHeight(textureHeight),
	m_CurrentNodeId(0),
	m_CurrentStage(Stage::Invalid),
	m_HasHeader(false)
	{
	}
	void BlueprintNodeBuilder::Begin(ed::NodeId id)
	{
		m_HasHeader = false;//draw every node, will clear this flag
		m_HeaderMin = m_HeaderMax = ImVec2();

		ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));

		ed::BeginNode(id);

		ImGui::PushID(id.AsPointer());

		m_CurrentNodeId = id;

		SetStage(Stage::Begin);

		//SetStage(Stage::Begin);
	}
	void BlueprintNodeBuilder::End()
	{
		SetStage(Stage::End);

		ed::EndNode();

		if (ImGui::IsItemVisible())
		{
			auto alpha = static_cast<int>(255 * ImGui::GetStyle().Alpha);

			auto drawList = ed::GetNodeBackgroundDrawList(m_CurrentNodeId);

			const auto halfBorderWidth = ed::GetStyle().NodeBorderWidth * 0.5f;

			auto headerColor = IM_COL32(0, 0, 0, alpha) | (m_HeaderColor & IM_COL32(255, 255, 255, 0));

			if ((m_HeaderMax.x > m_HeaderMin.x) && (m_HeaderMax.y > m_HeaderMin.y) && m_HeaderTextureWidth)
			{
				const auto uv = ImVec2(
					(m_HeaderMax.x - m_HeaderMin.x) / (float)(4.0f * m_HeaderTextureWidth),
					(m_HeaderMax.y - m_HeaderMin.y) / (float)(4.0f * m_HeaderTextureHeight));

				drawList->AddImageRounded(m_HeaderTextureId,
					m_HeaderMin - ImVec2(8 - halfBorderWidth, 4 - halfBorderWidth),
					m_HeaderMax + ImVec2(8 - halfBorderWidth, 0),
					ImVec2(0.0f, 0.0f), uv,
					headerColor, ed::GetStyle().NodeRounding, 1 | 2);

				auto headerSeparatorMin = ImVec2(m_HeaderMin.x, m_HeaderMax.y);
				auto headerSeparatorMax = ImVec2(m_HeaderMax.x, m_HeaderMin.y);

				if ((headerSeparatorMax.x > headerSeparatorMin.x) && (headerSeparatorMax.y > headerSeparatorMin.y))
				{
					drawList->AddLine(
						headerSeparatorMin + ImVec2(-(8 - halfBorderWidth), -0.5f),
						headerSeparatorMax + ImVec2((8 - halfBorderWidth), -0.5f),
						ImColor(255, 255, 255, 96 * alpha / (3 * 255)), 1.0f);
				}
			}
		}

		m_CurrentNodeId = 0;

		ImGui::PopID();

		ed::PopStyleVar();

		SetStage(Stage::Invalid);
	}

	void BlueprintNodeBuilder::Header(const ImVec4& color)
	{
		m_HeaderColor = ImColor(color);
		SetStage(Stage::Header);
	}

	void BlueprintNodeBuilder::EndHeader()
	{
		SetStage(Stage::Content);//begin content
	}

	void BlueprintNodeBuilder::Input(ed::PinId id)
	{
		if (m_CurrentStage == Stage::Begin)//don't have header
			SetStage(Stage::Content);

		bool applyPadding = (m_CurrentStage == Stage::Input);

		SetStage(Stage::Input);

		if (applyPadding)
			ImGui::Spring(0);

		Pin(id, ed::PinKind::Input);

		ImGui::BeginHorizontal(id.AsPointer());
	}

	void BlueprintNodeBuilder::EndInput()
	{
		ImGui::EndHorizontal();

		EndPin();
	}

	void BlueprintNodeBuilder::Output(ed::PinId id)
	{
		if (m_CurrentStage == Stage::Begin)
			SetStage(Stage::Content);

		bool applyPadding = (m_CurrentStage == Stage::Output);

		SetStage(Stage::Output);

		if (applyPadding)
			ImGui::Spring(0);

		Pin(id, ed::PinKind::Output);

		ImGui::BeginHorizontal(id.AsPointer());
	}

	void BlueprintNodeBuilder::EndOutput()
	{
		ImGui::EndHorizontal();

		EndPin();
	}

	void BlueprintNodeBuilder::Pin(ed::PinId id, ed::PinKind kind)
	{
		ed::BeginPin(id, kind);
	}

	void BlueprintNodeBuilder::EndPin()
	{
		ed::EndPin();
	}

	void BlueprintNodeBuilder::Middle()
	{
		//from begin to content to content
		if (m_CurrentStage == Stage::Begin)
			SetStage(Stage::Content);

		SetStage(Stage::Middle);
	}

	bool BlueprintNodeBuilder::SetStage(Stage newStage)
	{
		if (newStage == m_CurrentStage)
			return false;

		Stage oldStage = m_CurrentStage;
		m_CurrentStage = newStage;

		//widgets new pos
		ImVec2 cursor;
		switch(oldStage)
		{
		case Stage::Begin:
			break;

		case Stage::Header:
			ImGui::EndHorizontal();

			m_HeaderMin = ImGui::GetItemRectMin();
			m_HeaderMax = ImGui::GetItemRectMax();

			//insert spacing between header and content
			ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2.0f);
			break;
		case Stage::Content:
			//EndHorizontal?
			break;
		case Stage::Input:
			ed::PopStyleVar(2);

			ImGui::Spring(1, 0);

			ImGui::EndVertical();
			break;
		case Stage::Middle:
			ImGui::EndVertical();

			break;
		case Stage::Output:
			ed::PopStyleVar(2);

			ImGui::Spring(1, 0);

			ImGui::EndVertical();
			break;

		case Stage::End:
			break;
		case Stage::Invalid:
			break;
		}

		switch (newStage)
		{
		case Stage::Begin:
			ImGui::BeginVertical("node");
			break;
		case Stage::Header:
			m_HasHeader = true;
			ImGui::BeginHorizontal("header");
			break;
		case Stage::Content:
			if (oldStage == Stage::Begin)//don't have header
				ImGui::Spring(0);
			ImGui::BeginHorizontal("content");
			ImGui::Spring(0, 0);
			break;
		case Stage::Input:
			ImGui::BeginVertical("inputs", ImVec2(0, 0), 0.0f);

			ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(0, 0.5f));
			ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));

			if (!m_HasHeader)
				ImGui::Spring(1, 0);
			break;

		case Stage::Middle:
			ImGui::Spring(1);
			ImGui::BeginVertical("middle", ImVec2(0, 0), 1.0f);
			break;

		case Stage::Output:
			if (oldStage == Stage::Middle || oldStage == Stage::Input)
				ImGui::Spring(1);
			else
				ImGui::Spring(1, 0);
			ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f);

			ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(1.0f, 0.5f));
			ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));

			if (!m_HasHeader)
				ImGui::Spring(1, 0);
			break;

		case Stage::End:
			if (oldStage == Stage::Input)
				ImGui::Spring(1, 0);
			if (oldStage != Stage::Begin)
				ImGui::EndHorizontal();//content's horizontal
			m_ContentMin = ImGui::GetItemRectMin();
			m_ContentMax = ImGui::GetItemRectMax();

			//ImGui::Spring(0);
			ImGui::EndVertical();
			m_NodeMin = ImGui::GetItemRectMin();
			m_NodeMax = ImGui::GetItemRectMax();
			break;

		case Stage::Invalid:
			break;
		}

		return true;
	}

	
}
