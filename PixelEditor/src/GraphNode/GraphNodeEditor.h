#pragma once

#include "GraphPin.h"
#include "imgui-node-editor/imgui_node_editor.h"

namespace Pixel {
	namespace ed = ax::NodeEditor;

	class BlueprintNodeBuilder;
	class Material;
	class GraphLink;
	class GraphNode;
	class GraphPin;
	class Texture2D;
	class DescriptorHandle;
	class SimpleScene;
	class Timestep;
	class EditorCamera;
	class Framebuffer;
	class GraphNodeEditor {
	public:

		GraphNodeEditor(const std::string& virtualPath, const std::string& physicalPath, Ref<Material> pMaterial, Ref<Framebuffer> pFinalColorFrameBuffer);

		~GraphNodeEditor();

		void OnImGuiRender(bool& OpenGraphNodeEditor);

		void OnUpdate(Timestep& ts, EditorCamera& editorCamera, Ref<Framebuffer> pGeoFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, Ref<Framebuffer> pFinalFrameBuffer);

		bool IsPreviewSceneEventBlocked();

	private:

		void DrawMainFunctionNode();

		void DrawTopPanel(float panelHeight);

		void DrawLeftPreViewScenePanel(float panelWidth);

		void DrawLinks();

		void DrawNodes();

		void DrawPinIcon(ImU32 color, ImU32 innerColor);

		void HandleInteraction();

		void ShowLabel(const std::string& label, ImColor color);

		void CreateNewNodes();

		void ShowNodeContextMenu();

		//------create logic nodes------
		void CreateMul();

		void CreateConstFloatValue4();

		void CreateTexture2D(uint32_t textureParameterSuffix);
		//------create logic nodes------

		Ref<GraphPin> FindPin(ed::PinId pinId);

		ed::EditorContext* m_Editor = nullptr;

		std::string m_GraphNodeEditorConfigPath;

		std::string m_MaterialPhysicalPath;

		std::vector<Ref<GraphNode>> m_GraphNodes;
		std::vector<Ref<GraphPin>> m_GraphPins;
		std::vector<Ref<GraphLink>> m_GraphLinks;

		Ref<Material> m_pMaterial;

		Ref<SimpleScene> m_pPreviewScene;
		
		//uint32_t m_Id;

		//------header texture------
		Ref<Texture2D> m_HeaderBackgroundTexture;
		Ref<DescriptorHandle> m_HeaderBackgroundTextureHandle;
		//------header texture------
		Ref<BlueprintNodeBuilder> m_BlueprintNodeBuilder;

		//------preview scene texture------
		Ref<DescriptorHandle> m_PreviewSceneTextureHandle;
		//------preview scene texture------
		float m_TopPanelHeight;
		float m_DownPanelHeight;

		float m_LeftPaneWidth;
		float m_RightPanelWidth;

		bool m_CreateNewNode;

		//------current selected graph node------
		Ref<GraphNode> m_CurrentSelectedGraphNode;
		//------current selected graph node------

		bool m_IsPreviewEventBlocked;

		uint32_t textureParameterSuffix = 0;
	};
}
