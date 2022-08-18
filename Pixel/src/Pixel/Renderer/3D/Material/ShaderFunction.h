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
			ConstFloat4
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

		//------get node name------
		std::string GetShowName();
		//------get node name------

		ShaderFunctionType GetFunctionType();

		glm::vec2 GetFunctioNodePos();
		void SetFunctionNodePos(glm::vec2 pos);

		void SetShowName(const std::string& showName);

		//in terms of index get node
		Ref<InputNode> GetInputNode(uint32_t nodeId) const;
		//in terms of nodename get node
		Ref<InputNode> GetInputNode(const std::string& NodeName) const;

		Ref<OutputNode> GetOutputNode(uint32_t nodeId) const;
		Ref<OutputNode> GetOutputNode(const std::string& NodeName) const;

	public:
		virtual bool GetInputValueString(std::string& OutString) const;
		virtual bool GetOutputValueString(std::string& OutString) const;
		virtual bool GetFunctionString(std::string& OutString) const;

		//------this function not be used------
		//------Reset Value Type(Max Value Type transformation)------//
		virtual bool ResetValueType() const;
		//------Reset Value Type(Max Value Type transformation)------//
		bool GetShaderTreeString(std::string& OutString);
		//------this function not be used------
		bool ClearShaderTreeStringFlag();

		std::string GetValueEqualString(const Ref<OutputNode> pOutputNode, const Ref<InputNode> pInputNode) const;

		void AddToMaterialOwner();
		virtual void ConstructPutNodeAndSetPutNodeOwner();
	protected:
		//dfs check flag
		bool m_bIsVisited;
	};
}
