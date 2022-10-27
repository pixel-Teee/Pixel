#pragma once

#include <glm/glm.hpp>

namespace Pixel {
	class Material;

	class InputNode;

	class OutputNode;
	//enum class Meta(Enable) ShaderFunctionType {
	//	ConstFloatValue4 = 0,
	//	Mul = 1,
	//	Main = 2,
	//};
	//node logic
	class Meta(Enable) ShaderFunction : public std::enable_shared_from_this<ShaderFunction>
	{
	public:
		
	protected:
		Meta()
		//node name
		std::string m_ShowName;

		ShaderFunction(const std::string& ShowName, Ref<Material> pMaterial);

		Meta()
		//Input
		std::vector<Ref<InputNode>> m_pInputs;

		Meta()
		//Output
		std::vector<Ref<OutputNode>> m_pOutputs;

		//Owner material
		std::weak_ptr<Material> m_pOwner;

		Meta()
		//node id
		uint32_t m_NodeId;
	public:
		Meta()
		ShaderFunction() = default;

		virtual ~ShaderFunction();

		//------check have input and output node and get the number------//
		bool IsHaveOutput();

		bool IsHaveInput();

		uint32_t GetInputNodeNum();

		uint32_t GetOutputNodeNum();
		//------check have input and output node and get the number------//

		//------get node name and set node name------
		std::string GetShowName();

		void SetShowName(const std::string& showName);
		//------get node name and set node name------

		//ShaderFunctionType GetFunctionType();

		//------get node and set node id------
		uint32_t GetFunctioNodeId();

		void SetFunctionNodeId(uint32_t nodeId);
		//------get node and set node id------

		//in terms of index get node
		Ref<InputNode> GetInputNode(uint32_t nodeId) const;

		//in terms of node name get node
		Ref<InputNode> GetInputNode(const std::string& NodeName) const;

		Ref<OutputNode> GetOutputNode(uint32_t nodeId) const;

		Ref<OutputNode> GetOutputNode(const std::string& NodeName) const;

		virtual bool GetInputValueString(std::string& OutString) const;

		virtual bool GetOutputValueString(std::string& OutString) const;

		virtual bool GetFunctionString(std::string& OutString) const;

		std::string GetValueEqualString(const Ref<OutputNode> pOutputNode, const Ref<InputNode> pInputNode) const;

		virtual bool ResetValueType() const;

		//only give the main shader function use
		virtual bool GetShaderTreeString(std::string& OutString);

		bool ClearShaderTreeStringFlag();

		void AddToMaterialOwner();

		virtual void ConstructPutNodeAndSetPutNodeOwner();

		void ClearVisit();//TODO:need to remove this

		std::vector<std::string> m_InputNodeDisplayName;

		std::vector<glm::vec3> m_InputNodeDisplayColor;
		
		std::vector<std::string> m_OutputNodeDisplayName;

		std::vector<glm::vec3> m_OutputNodeDisplayColor;

		glm::vec3 m_HeaderColor;

		std::string m_IntermediateShaderString;//use for intermediate nodes
	protected:
		//dfs check flag
		bool m_bIsVisited;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
		friend class Material;
	};
}
