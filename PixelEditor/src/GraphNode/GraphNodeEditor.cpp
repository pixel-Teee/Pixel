#include "pxpch.h"

#include "GraphNodeEditor.h"

namespace Pixel {

	GraphNodeEditor::GraphNodeEditor()
	{
		m_FirstOpen = true;
	}

	GraphNodeEditor::~GraphNodeEditor()
	{
	}

	void GraphNodeEditor::OnImGuiRender(bool& OpenGraphNodeEditor)
	{
		//when open, will from the material asset open, then load the grah editor json file
		
		ImGui::Begin("Graph Node Editor", &OpenGraphNodeEditor);

		if (OpenGraphNodeEditor)
		{
			if (m_FirstOpen)
			{
				//if is first open, then load the settings file
				ed::Config config;

				config.SettingsFile = "assets\\GraphNodeEditorTest.json";

				m_Editor = ed::CreateEditor(&config);

				m_FirstOpen = false;
			}

			//render
			ed::SetCurrentEditor(m_Editor);
				ed::Begin("Graph Node Editor Canvas", ImVec2(0.0f, 0.0f));
				
				

				ed::End();
			ed::SetCurrentEditor(nullptr);
		}
		else
			m_FirstOpen = true;

		ImGui::End();
	}

	void GraphNodeEditor::DrawPbrNode()
	{
	}

}