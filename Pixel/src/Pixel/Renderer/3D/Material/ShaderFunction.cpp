#include "pxpch.h"

//------my library------
#include "ShaderFunction.h"
#include "ShaderStringFactory.h"
#include "InputNode.h"
#include "OutputNode.h"
#include "Material.h"
//------my library------

namespace Pixel {

	ShaderFunction::ShaderFunction(const std::string& ShowName, Ref<Material> pMaterial)
	{
		m_bIsVisited = false;
		//------node name------
		m_ShowName = ShowName;
		//------node name------
		m_pInputs.clear();
		m_pOutputs.clear();
		m_pOwner = pMaterial;
	}

	ShaderFunction::~ShaderFunction()
	{
		m_pInputs.clear();
		m_pOutputs.clear();
		Ref<Material> pMaterial = m_pOwner.lock();
		if (pMaterial != nullptr)
		{
			pMaterial->DeleteShaderFunction(shared_from_this());
		}
	}

	bool ShaderFunction::IsHaveOutput()
	{
		return m_pOutputs.size() != 0;
	}

	bool ShaderFunction::IsHaveInput()
	{
		return m_pInputs.size() != 0;
	}

	uint32_t ShaderFunction::GetInputNodeNum()
	{
		return m_pInputs.size();
	}

	uint32_t ShaderFunction::GetOutputNodeNum()
	{
		return m_pOutputs.size();
	}

	void ShaderFunction::AddToMaterialOwner()
	{
		if(m_pOwner.expired())
		{
			std::shared_ptr<Material> pMaterial = m_pOwner.lock();

			pMaterial->AddShaderFunction(shared_from_this());
		}
	}

	void ShaderFunction::ConstructPutNodeAndSetPutNodeOwner()
	{

	}

	std::string ShaderFunction::GetShowName()
	{
		return m_ShowName;
	}

	ShaderFunction::ShaderFunctionType ShaderFunction::GetFunctionType()
	{
		return m_functionType;
	}

	uint32_t ShaderFunction::GetFunctioNodeId()
	{
		return m_NodeId;
	}

	void ShaderFunction::SetFunctionNodeId(uint32_t nodeId)
	{
		m_NodeId = nodeId;
	}

	void ShaderFunction::SetShowName(const std::string& showName)
	{
		m_ShowName = showName;
	}

	Ref<InputNode> ShaderFunction::GetInputNode(uint32_t nodeId) const
	{
		return m_pInputs[nodeId];
	}

	Ref<InputNode> ShaderFunction::GetInputNode(const std::string& NodeName) const
	{
		for (uint32_t i = 0; i < m_pInputs.size(); ++i)
		{
			if (m_pInputs[i]->GetNodeName() == NodeName)
				return m_pInputs[i];
		}
		return nullptr;
	}

	Ref<OutputNode> ShaderFunction::GetOutputNode(uint32_t nodeId) const
	{
		return m_pOutputs[nodeId];
	}

	Ref<OutputNode> ShaderFunction::GetOutputNode(const std::string& NodeName) const
	{
		for (uint32_t i = 0; i < m_pOutputs.size(); ++i)
		{
			if (m_pOutputs[i]->GetNodeName() == NodeName)
				return m_pOutputs[i];
		}
		return nullptr;
	}

	//get the input temp variable declare
	bool ShaderFunction::GetInputValueString(std::string& OutString) const
	{
		std::string Temp;

		//traverse all input
		for (uint32_t i = 0; i < m_pInputs.size(); ++i)
		{
			//check type
			if (m_pInputs[i]->GetValueType() == PutNode::ValueType::VT_1)
			{
				//get the type
				OutString += ShaderStringFactory::Float() + (" ");

				//get the default value
				Temp = ShaderStringFactory::FloatConst("0");
			}
			else if (m_pInputs[i]->GetValueType() == PutNode::ValueType::VT_2)
			{
				//get the Type
				OutString += ShaderStringFactory::Float2() + (" ");

				Temp = ShaderStringFactory::FloatConst2("0", "0");
			}
			else if (m_pInputs[i]->GetValueType() == PutNode::ValueType::VT_3)
			{
				OutString += ShaderStringFactory::Float3() + (" ");

				Temp = ShaderStringFactory::FloatConst3("0", "0", "0");
			}
			else
			{
				OutString += ShaderStringFactory::Float4() + (" ");

				Temp = ShaderStringFactory::FloatConst4("0", "0", "0", "1");
			}

			//don't have any output node link the input node
			if (!m_pInputs[i]->GetOutputLink())
			{
				OutString += m_pInputs[i]->GetNodeName() + " = " + Temp + ";\n";//float4 InputNodeName = float4(0, 0, 0, 1);
				continue;
			}

			//else, let the input node equal to the output node
			OutString += GetValueEqualString(m_pInputs[i]->GetOutputLink(), m_pInputs[i]);
		}
		return true;
	}

	bool ShaderFunction::GetOutputValueString(std::string& OutString) const
	{
		std::string Temp;
		for (uint32_t i = 0; i < m_pOutputs.size(); ++i)
		{
			if (m_pOutputs[i]->GetValueType() == PutNode::ValueType::VT_1)
			{
				OutString += ShaderStringFactory::Float() + " ";

				Temp = ShaderStringFactory::FloatConst("0");//0
			}
			else if (m_pOutputs[i]->GetValueType() == PutNode::ValueType::VT_2)
			{
				//Get the Type
				OutString += ShaderStringFactory::Float2() + (" ");

				Temp = ShaderStringFactory::FloatConst2("0", "0");//float2(0, 0)
			}
			else if (m_pOutputs[i]->GetValueType() == PutNode::ValueType::VT_3)
			{
				OutString += ShaderStringFactory::Float3() + (" ");

				Temp = ShaderStringFactory::FloatConst3("0", "0", "0");//float3(0, 0, 0)
			}
			else
			{
				OutString += ShaderStringFactory::Float4() + (" ");

				Temp = ShaderStringFactory::FloatConst4("0", "0", "0", "1");//float4(0, 0, 0, 1)
			}

			OutString += m_pOutputs[i]->GetNodeName() + " = " + Temp + ";\n";//float4 OutputNodeName = float4(0, 0, 0, 1);
		}
		return true;
	}

	//get the node expression, calculate temp input variable value, assign to temp output variable value
	bool ShaderFunction::GetFunctionString(std::string& OutString) const
	{
		return true;
	}

	bool ShaderFunction::ResetValueType() const
	{
		return true;
	}

	bool ShaderFunction::GetShaderTreeString(std::string& OutString)
	{
		if (m_bIsVisited == true)
			return true;
		else
		{
			m_bIsVisited = true;
			for (uint32_t i = 0; i < m_pInputs.size(); ++i)
			{
				if (m_pInputs[i]->GetOutputLink() == nullptr)
					continue;
				else
				{
					m_pInputs[i]->GetOutputLink()->GetOwner()->GetShaderTreeString(OutString);
				}
			}

			//TODO:reset input type and output type
			if (!ResetValueType())
			{
				return false;
			}

			//get input value declare
			if (!GetInputValueString(OutString))
				return false;
			//get output value declare
			if (!GetOutputValueString(OutString))
				return false;
			//get function string
			if (!GetFunctionString(OutString))
				return false;

			return true;
		}
	}

	//from the main calculate light node to clear flag
	bool ShaderFunction::ClearShaderTreeStringFlag()
	{
		//recursive clear
		if (m_bIsVisited == false)
			return true;
		else
		{
			m_bIsVisited = false;
			for (uint32_t i = 0; i < m_pInputs.size(); ++i)
			{
				if (m_pInputs[i]->GetOutputLink() == nullptr)
					continue;
				else
				{
					//recursive clear
					(std::static_pointer_cast<ShaderFunction>(m_pInputs[i]->GetOutputLink()->GetOwner()))->ClearShaderTreeStringFlag();
				}
			}
		}

		return true;
	}

	std::string ShaderFunction::GetValueEqualString(const Ref<OutputNode> pOutputNode, const Ref<InputNode> pInputNode) const
	{
		if (pOutputNode == nullptr || pInputNode == nullptr)
			return "";

		//get the input type
		PutNode::ValueType maxElement = pInputNode->GetValueType();

		//if input type equal to the output type, then input equal to output
		if (maxElement == pOutputNode->GetValueType())
		{
			return pInputNode->GetNodeName() + " = " + pOutputNode->GetNodeName() + ";\n";
		}

		std::string OutString;
		OutString = pInputNode->GetNodeName() + " = ";

		std::string Value[4];
		ShaderStringFactory::ValueElement Mask[4];
		Mask[0] = ShaderStringFactory::ValueElement::VE_R;
		Mask[1] = ShaderStringFactory::ValueElement::VE_G;
		Mask[2] = ShaderStringFactory::ValueElement::VE_B;
		Mask[3] = ShaderStringFactory::ValueElement::VE_A;

		for (uint32_t i = 0; i < 4; ++i)
		{
			if (i > pOutputNode->GetValueType())
			{
				Value[i] = ShaderStringFactory::GetValueElement(pOutputNode, Mask[pOutputNode->GetValueType()]);
			}
			else
			{
				Value[i] = ShaderStringFactory::GetValueElement(pOutputNode, Mask[i]);
			}
		}

		//similar to float4(xx.r, xx.r, xx.r, xx.r);
		if (pInputNode->GetValueType() == PutNode::ValueType::VT_1)
		{
			OutString += ShaderStringFactory::FloatConst(Value[0]);
		}
		else if (pInputNode->GetValueType() == PutNode::ValueType::VT_2)
		{
			OutString += ShaderStringFactory::FloatConst2(Value[0], Value[1]);
		}
		else if (pInputNode->GetValueType() == PutNode::ValueType::VT_3)
		{
			OutString += ShaderStringFactory::FloatConst3(Value[0], Value[1], Value[2]);
		}
		else if(pInputNode->GetValueType() == PutNode::ValueType::VT_4)
		{
			OutString += ShaderStringFactory::FloatConst4(Value[0], Value[1], Value[2], Value[3]);
		}
		else
		{
			return "";
		}
		OutString += ";\n";
		return OutString;
	}

	RTTR_REGISTRATION
	{
		using namespace rttr;
		registration::class_<ShaderFunction>("ShaderFunction")
			.constructor<const std::string&, Ref<Material>>()
			.property("m_ShowName", &ShaderFunction::m_ShowName)
			.property("m_functionType", &ShaderFunction::m_functionType)
			.property("m_pInputs", &ShaderFunction::m_pInputs)
			.property("m_pOutputs", &ShaderFunction::m_pOutputs)
			.property("m_NodeId", &ShaderFunction::m_NodeId);
	}
}