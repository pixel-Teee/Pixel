#pragma once

#include <glm/glm.hpp>

namespace Pixel {
	class Material;

	class InputNode;

	class OutputNode;
	//node logic
	class ShaderFunction : public std::enable_shared_from_this<ShaderFunction>
	{
	public:
		enum class ShaderFunctionType {
			ConstFloat4,
			Mul
		};
	protected:
		//node name
		std::string m_ShowName;

		//function type
		ShaderFunctionType m_functionType;

		ShaderFunction(const std::string& ShowName, Ref<Material> pMaterial);

		//Input
		std::vector<Ref<InputNode>> m_pInputs;

		//Output
		std::vector<Ref<OutputNode>> m_pOutputs;

		//Owner material
		std::weak_ptr<Material> m_pOwner;

		//editor:need to fix
		glm::vec2 m_Pos;
	public:
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

		ShaderFunctionType GetFunctionType();

		//------get node and set node pos------
		glm::vec2 GetFunctioNodePos();

		void SetFunctionNodePos(glm::vec2 pos);
		//------get node and set node pos------

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
		bool GetShaderTreeString(std::string& OutString);

		bool ClearShaderTreeStringFlag();

		void AddToMaterialOwner();

		virtual void ConstructPutNodeAndSetPutNodeOwner();

	protected:
		//dfs check flag
		bool m_bIsVisited;
	};
}