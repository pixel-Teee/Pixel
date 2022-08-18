#include "pxpch.h"

//------my library------
#include "ConstFloatValue.h"
#include "OutputNode.h"
#include "ShaderStringFactory.h"
//------my library------

namespace Pixel
{
	//------ConstFloatValue------
	ConstFloatValue::ConstFloatValue(const std::string& showName, Ref<Material> pMaterial, uint32_t valueNumber, bool bIsCustom)
		:ConstValue(showName, pMaterial, valueNumber, bIsCustom)
	{
		m_valueNumber = valueNumber;
		m_bIsCustom = bIsCustom;
	}

	void ConstFloatValue::ConstrcutPutNodeAndSetPutNodeOwner()
	{
		//create temp output variable
		std::string OutputID = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "ConstFloatValue" + OutputID;

		Ref<OutputNode> pOutputNode;
		pOutputNode = CreateRef<OutputNode>((PutNode::ValueType)(m_valueNumber - 1), OutputName, shared_from_this());
		m_pOutputs.push_back(pOutputNode);

		//create temp variable
		//if temp variable ConstFloatValue have 4 components
		//and will create 4 temp variable
		//ConstFloatValueID.x 
		//ConstFloatValueID.y
		//ConstFloatValueID.z
		//ConstFloatValueID.w

		++ShaderStringFactory::m_ShaderValueIndex;
		if (m_valueNumber >= 1)
		{
			std::string OutputNameR = ShaderStringFactory::GetValueElement(GetOutputNode(OUT_VALUE), ShaderStringFactory::VE_R);//PutNode name just ConstFloatValueID.x
			Ref<OutputNode> pOutputNode;
			pOutputNode = CreateRef<OutputNode>(PutNode::ValueType::VT_1, OutputNameR, shared_from_this());
			m_pOutputs.push_back(pOutputNode);
		}

		if (m_valueNumber >= 2)
		{
			std::string OutputNameG = ShaderStringFactory::GetValueElement(GetOutputNode(OUT_VALUE), ShaderStringFactory::VE_G);
			Ref<OutputNode> pOutputNode;
			pOutputNode = CreateRef<OutputNode>(PutNode::ValueType::VT_1, OutputNameG, shared_from_this());
			m_pOutputs.push_back(pOutputNode);
		}

		if (m_valueNumber >= 3)
		{
			std::string OutputNameB = ShaderStringFactory::GetValueElement(GetOutputNode(OUT_VALUE), ShaderStringFactory::VE_B);
			Ref<OutputNode> pOutputNode;
			pOutputNode = CreateRef<OutputNode>(PutNode::ValueType::VT_1, OutputNameB, shared_from_this());
			m_pOutputs.push_back(pOutputNode);
		}

		if (m_valueNumber >= 4)
		{
			std::string OutputNameA = ShaderStringFactory::GetValueElement(GetOutputNode(OUT_VALUE), ShaderStringFactory::VE_A);
			Ref<OutputNode> pOutputNode;
			pOutputNode = CreateRef<OutputNode>(PutNode::ValueType::VT_1, OutputNameA, shared_from_this());
			m_pOutputs.push_back(pOutputNode);
		}
		m_Value.clear();
		m_Value.resize(m_valueNumber);
		for (uint32_t i = 0; i < m_valueNumber; ++i)
		{
			m_Value[i] = 0;
		}
	}

	void ConstFloatValue::SetValue(uint32_t index, float value)
	{
		PX_CORE_ASSERT(index <= m_Value.size(), "index out of range!");

		m_Value[index] = value;
	}

	bool ConstFloatValue::GetOutputValueString(std::string& OutString) const
	{
		if (m_bIsCustom)
			return true;
		if (m_pOutputs[0]->GetValueType() == PutNode::ValueType::VT_1)
		{
			OutString += ShaderStringFactory::Float() + " ";
		}
		else if (m_pOutputs[0]->GetValueType() == PutNode::ValueType::VT_2)
		{
			OutString += ShaderStringFactory::Float2() + " ";
		}
		else if (m_pOutputs[0]->GetValueType() == PutNode::ValueType::VT_3)
		{
			OutString += ShaderStringFactory::Float3() += " ";
		}
		else if (m_pOutputs[0]->GetValueType() == PutNode::ValueType::VT_4)
		{
			OutString += ShaderStringFactory::Float4() += " ";
		}

		OutString += m_pOutputs[0]->GetNodeName() += ";\n";
		return true;
	}

	bool ConstFloatValue::GetFunctionString(std::string& OutString) const
	{
		if (m_bIsCustom)
			return true;
		//if don't provided from user, then use constant value
		std::string Value[4];
		for (uint32_t i = 0; i < m_Value.size(); ++i)
		{
			Value[i] = std::to_string(m_Value[i]);
		}
		OutString += m_pOutputs[0]->GetNodeName() + " = ";
		if (m_pOutputs[0]->GetValueType() == PutNode::VT_1)
		{
			OutString += ShaderStringFactory::FloatConst(Value[0]);
		}
		else if (m_pOutputs[0]->GetValueType() == PutNode::VT_2)
		{
			OutString += ShaderStringFactory::FloatConst2(Value[0], Value[1]);
		}
		else if (m_pOutputs[0]->GetValueType() == PutNode::VT_3)
		{
			OutString += ShaderStringFactory::FloatConst3(Value[0], Value[1], Value[2]);
		}
		else if (m_pOutputs[0]->GetValueType() == PutNode::VT_4)
		{
			OutString += ShaderStringFactory::FloatConst4(Value[0], Value[1], Value[2], Value[3]);
		}
		OutString += ";\n";
		return true;
	}

	//give the main header use 
	bool ConstFloatValue::GetDeclareString(std::string& OutString, uint32_t location) const
	{
		//could in material provide, then declare in header top, uniform
		if (!m_bIsCustom)
			return true;
		if (m_pOutputs[0]->GetValueType() == PutNode::VT_1)
		{
			OutString += ShaderStringFactory::Float() + " ";
		}
		else if (m_pOutputs[1]->GetValueType() == PutNode::VT_2)
		{
			OutString += ShaderStringFactory::Float2() + " ";
		}
		else if (m_pOutputs[2]->GetValueType() == PutNode::VT_3)
		{
			OutString += ShaderStringFactory::Float3() + " ";
		}
		else if (m_pOutputs[3]->GetValueType() == PutNode::VT_4)
		{
			OutString += ShaderStringFactory::Float4() += " ";
		}
		else
			return false;

		//uniform vec4 xx;
		OutString += m_pOutputs[0]->GetNodeName() + (";\n");
		return true;
	}

	void ConstFloatValue::ResetInShaderName()
	{
		//reset name
		std::string OutputId = std::to_string(ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "ConstFloatValue" + OutputId;
		m_pOutputs[0]->SetNodeName(OutputName);
		++ShaderStringFactory::m_ShaderValueIndex;
		if (m_Value.size() >= 1)
		{
			std::string OutputNameR = ShaderStringFactory::GetValueElement(GetOutputNode(OUT_VALUE), ShaderStringFactory::VE_R);
			Ref<OutputNode> pOutputNode;
			m_pOutputs[0]->SetNodeName(OutputNameR);
		}

		if (m_Value.size() >= 2)
		{
			std::string OutputNameG = ShaderStringFactory::GetValueElement(GetOutputNode(OUT_VALUE), ShaderStringFactory::VE_G);
			Ref<OutputNode> pOutputNode;
			pOutputNode = CreateRef<OutputNode>(PutNode::ValueType::VT_1, OutputNameG, std::shared_ptr<ShaderFunction>(shared_from_this()));
			m_pOutputs[0]->SetNodeName(OutputNameG);
		}

		if (m_Value.size() >= 3)
		{
			std::string OutputNameB = ShaderStringFactory::GetValueElement(GetOutputNode(OUT_VALUE), ShaderStringFactory::VE_B);
			Ref<OutputNode> pOutputNode;
			pOutputNode = CreateRef<OutputNode>(PutNode::ValueType::VT_1, OutputNameB, std::shared_ptr<ShaderFunction>(shared_from_this()));
			m_pOutputs[0]->SetNodeName(OutputNameB);
		}

		if (m_Value.size() >= 4)
		{
			std::string OutputNameA = ShaderStringFactory::GetValueElement(GetOutputNode(OUT_VALUE), ShaderStringFactory::VE_A);
			Ref<OutputNode> pOutputNode;
			pOutputNode = CreateRef<OutputNode>(PutNode::ValueType::VT_1, OutputNameA, std::shared_ptr<ShaderFunction>(shared_from_this()));
			m_pOutputs[0]->SetNodeName(OutputNameA);
		}
	}
	//------ConstFloatValue------
}