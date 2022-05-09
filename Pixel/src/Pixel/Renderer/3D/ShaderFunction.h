#pragma once

#include "Pixel/Core/Core.h"
#include "PutNode.h"

#include "glm/glm.hpp"

#include <string>

namespace Pixel {
	class Material;
	//node logic
	class ShaderFunction : public std::enable_shared_from_this<ShaderFunction>
	{
	public:
		enum ShaderFunctionType {
			GeoMetry,
			ConstFloat4,
			Mul,
			Sampler2D
		};
	protected:
		//node name
		std::string m_ShowName;
		//function type
		ShaderFunctionType m_functionType;
		ShaderFunction(const std::string& ShowName, Ref<Material> pMaterial);
		//Input
		std::vector<Ref<InputNode>> m_pInput;
		//Output
		std::vector<Ref<OutputNode>> m_pOutput;

		//Owner material
		Ref<Material> m_pOwner;

		//editor:need to fix
		glm::vec2 m_Pos;
	public:
		ShaderFunction() = default;
		virtual ~ShaderFunction();

		//------check have input and output node and get the number------//
		bool IsHaveOutput()
		{
			return m_pOutput.size() != 0;
		}

		bool IsHaveInput()
		{
			return m_pInput.size() != 0;
		}

		uint32_t GetInputNodeNum()
		{
			return m_pInput.size();
		}

		uint32_t GetOutputNodeNum()
		{
			return m_pOutput.size();
		}
		//------check have input and output node and get the number------//

		std::string GetShowName();

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
		virtual void ConstrcutPutNodeAndSetPutNodeOwner();
	protected:
		//dfs check flag
		bool m_bIsVisited;

		friend class SerializerMaterial;
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

		virtual bool GetInputValueString(std::string& OutString, uint32_t uiOutPutStringType);

		//virtual bool GetFunctionString(std::string& OutString) const override;

		virtual bool GetShaderTreeString(std::string& OutString, uint32_t uiOutPutStringType);

		//------four userful node------
		virtual Ref<InputNode> GetWorldPosNode() = 0;

		virtual Ref<InputNode> GetAlbedoNode() = 0;

		virtual Ref<InputNode> GetRoughness() = 0;

		virtual Ref<InputNode> GetMetallicNode() = 0;

		virtual Ref<InputNode> GetNormalNode() = 0;

		virtual Ref<InputNode> GetEmissiveNode() = 0;
		//------four userful node------

		//------non user declare type---
		void GetValueUseDeclareString(std::string& OutString, uint32_t uiValueUseString);
		//vec3 WorldNormal etc.
		virtual void GetValueUseString(std::string& OutString, uint32_t uiOutPutStringType);
		//------non user declare type---

		//------first get the main function normal node pinput node------
		void GetNormalString(std::string& OutString);
		//------first get the main function normal node pinput node------
	};

	//user provdied, will link to a UserConstant(just to assignment to real shader value) class
	class ConstValue : public ShaderFunction
	{
	public:
		//if bIsCustom, then will declare as uniform variable
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

		virtual void ConstrcutPutNodeAndSetPutNodeOwner();

		//set param value
		void SetValue(uint32_t index, float value);
		//similar to vec4 NodeNameID.Type
		virtual bool GetOutputValueString(std::string& OutString) const;
		//in terms of whether is user's uniform variable
		virtual bool GetFunctionString(std::string& OutString) const;
		//if ConstFloatValue is uniform, then registerId will use, but glsl don't have this
		//else don't use registerId
		bool GetDeclareString(std::string& OutString, uint32_t registerId) const;
		//before compile shader, will call this function reset temp variable name
		virtual void ResetInShaderName();

		std::vector<float> m_Value;

		//value number 
		uint32_t m_valueNumber;

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
