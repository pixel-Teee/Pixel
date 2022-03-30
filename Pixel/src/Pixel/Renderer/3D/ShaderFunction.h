#pragma once

#include "Pixel/Core/Core.h"
#include "PutNode.h"
#include <string>

namespace Pixel {
	class Material;
	//node logic
	class ShaderFunction : public std::enable_shared_from_this<ShaderFunction>
	{
	protected:
		//node name
		std::string m_ShowName;
		ShaderFunction(const std::string& ShowName, Ref<Material> pMaterial);
		//Input
		std::vector<Ref<InputNode>> m_pInput;
		//Output
		std::vector<Ref<OutputNode>> m_pOutput;

		//Owner material
		Ref<Material> m_pOwner;
	public:
		ShaderFunction() = default;
		virtual ~ShaderFunction();
		virtual bool IsHaveOutput()
		{
			//
			return true;
		}

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

		bool GetShaderTreeString(std::string& OutString);
		bool ClearShaderTreeStringFlag();

		std::string GetValueEqualString(const Ref<OutputNode> pOutputNode, const Ref<InputNode> pInputNode) const;
	protected:
		//dfs check flag
		bool m_bIsVisited;
	};

	class ShaderMainFunction : public ShaderFunction
	{
	public:
		enum OutputStringType
		{
			OST_MATERIAL,
			OST_MAX
		};

		enum ValueUseString
		{
			VUS_WORLD_POS = BIT(1),
			VUS_VIEW_POS = BIT(2) | VUS_WORLD_POS,
			VUS_PROJ_POS = BIT(3) | VUS_VIEW_POS,
			VUS_WORLD_NORMAL = BIT(4),
			VUS_VIEW_NORMAL = BIT(5) | VUS_WORLD_NORMAL,
			VUS_VIEW_WORLD_DIR = BIT(6),
			VUS_ALL = VUS_PROJ_POS | VUS_VIEW_POS | VUS_VIEW_WORLD_DIR
		};

		ShaderMainFunction() = default;
		virtual ~ShaderMainFunction() = default;
		void GetValueUseDeclareString(std::string& OutString, uint32_t uiValueUseString);

		virtual bool IsHaveOutput() override;

		virtual bool GetInputValueString(std::string& OutString, uint32_t uiOutPutStringType);

		virtual bool GetOutputValueString(std::string& OutString) const override;

		virtual bool GetFunctionString(std::string& OutString) const override;

		virtual bool GetShaderTreeString(std::string& OutString, uint32_t uiOutPutStringType);

		virtual Ref<InputNode> GetAlbedoNode() = 0;

		virtual Ref<InputNode> GetRoughness() = 0;

		virtual Ref<InputNode> GetMetallicNode() = 0;

		virtual Ref<InputNode> GetNormalNode() = 0;

		virtual void GetValueUseString(std::string& OutString, uint32_t uiOutPutStringType);
		void GetNormalString(std::string& OutString);
	};

	//user provdied, will link to a UserConstant class
	class ConstValue : public ShaderFunction
	{
	public:
		ConstValue(const std::string& showName, Ref<Material> pMaterial, uint32_t valueNumber, bool bIsCustom);

		virtual ~ConstValue();

		//If this is not user provided, then use default value
		bool m_bIsCustom;
	};

	class ConstFloatValue : public ConstValue
	{
	public:
		ConstFloatValue() = default;
		ConstFloatValue(const std::string& showName, Ref<Material> pMaterial, uint32_t valueNumber, bool bIsCustom);

		//set param value
		void SetValue(uint32_t index, float value);
		virtual bool GetOutputValueString(std::string& OutString) const;
		virtual bool GetFunctionString(std::string& OutString) const;
		//if ConstFloatValue is uniform, then registerId will use, but glsl don't have this
		//else don't use registerId
		bool GetDeclareString(std::string& OutString, uint32_t registerId) const;
		virtual void ResetInShaderName();

		std::vector<float> m_Value;

		enum OutValueElement
		{
			OUT_VALUE,
			OUT_VALUE_X,
			OUT_VALUE_Y,
			OUT_VALUE_Z,
			OUT_VALUE_w,
			OUT_MAX
		};
	};
}
