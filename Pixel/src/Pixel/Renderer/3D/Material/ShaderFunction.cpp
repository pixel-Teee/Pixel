#include "pxpch.h"

//------my library------
#include "ShaderFunction.h"
#include "InputNode.h"
#include "OutputNode.h"
#include "Material.h"
//------my library------

namespace Pixel {

	ShaderFunction::ShaderFunction(const std::string& ShowName, Ref<Material> pMaterial)
	{
		m_bIsVisited = false;
		m_ShowName = ShowName;
		m_pInputs.clear();
		m_pOutputs.clear();
		m_pOwner = pMaterial;
	}

	ShaderFunction::~ShaderFunction()
	{
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

	glm::vec2 ShaderFunction::GetFunctioNodePos()
	{
		return m_Pos;
	}

	void ShaderFunction::SetFunctionNodePos(glm::vec2 pos)
	{
		m_Pos = pos;
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
		for (uint32_t i = 0; i < m_pInput.size(); ++i)
		{
			//check type
			if (m_pInput[i]->GetValueType() == PutNode::VT_1)
			{
				//Get the Type
				OutString += Renderer3D::Float() + (" ");

				//Get the default value
				Temp = Renderer3D::FloatConst("0");
			}
			else if (m_pInput[i]->GetValueType() == PutNode::VT_2)
			{
				//Get the Type
				OutString += Renderer3D::Float2() + (" ");

				Temp = Renderer3D::FloatConst2("0", "0");
			}
			else if (m_pInput[i]->GetValueType() == PutNode::VT_3)
			{
				OutString += Renderer3D::Float3() + (" ");

				Temp = Renderer3D::FloatConst3("0", "0", "0");
			}
			else
			{
				OutString += Renderer3D::Float4() + (" ");

				Temp = Renderer3D::FloatConst4("0", "0", "0", "1");
			}

			//don't have any output node link the input node
			if (!m_pInput[i]->GetOutputLink())
			{
				OutString += m_pInput[i]->GetNodeName() + " = " + Temp + ";\n";
				continue;
			}

			//else, let the input node equal to the output node
			OutString += GetValueEqualString(m_pInput[i]->GetOutputLink(), m_pInput[i]);
		}
		return true;
	}

	bool ShaderFunction::GetOutputValueString(std::string& OutString) const
	{
		std::string Temp;
		for (uint32_t i = 0; i < m_pOutput.size(); ++i)
		{
			if (m_pOutput[i]->GetValueType() == CustomFloatValue::VT_1)
			{
				OutString += Renderer3D::Float() + " ";
				Temp = Renderer3D::FloatConst("0");
			}
			else if (m_pOutput[i]->GetValueType() == PutNode::VT_2)
			{
				//Get the Type
				OutString += Renderer3D::Float2() + (" ");

				Temp = Renderer3D::FloatConst2("0", "0");
			}
			else if (m_pOutput[i]->GetValueType() == PutNode::VT_3)
			{
				OutString += Renderer3D::Float3() + (" ");

				Temp = Renderer3D::FloatConst3("0", "0", "0");
			}
			else
			{
				OutString += Renderer3D::Float4() + (" ");

				Temp = Renderer3D::FloatConst4("0", "0", "0", "1");
			}

			OutString += m_pOutput[i]->GetNodeName() + " = " + Temp + ";\n";
		}
		return true;
	}

	//get the node expression, calculate temp input variable value, assign to output variable value
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
			for (uint32_t i = 0; i < m_pInput.size(); ++i)
			{
				if (m_pInput[i]->GetOutputLink() == nullptr)
					continue;
				else
				{
					(std::static_pointer_cast<ShaderFunction>(m_pInput[i]->GetOutputLink()->GetOwner()))->GetShaderTreeString(OutString);
				}
			}

			//TODO:reset input type and output type
			if (!ResetValueType())
			{
				return false;
			}
			
			if (!GetInputValueString(OutString))
				return false;
			if (!GetOutputValueString(OutString))
				return false;
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
			for (uint32_t i = 0; i < m_pInput.size(); ++i)
			{
				if (m_pInput[i]->GetOutputLink() == nullptr)
					continue;
				else
				{
					//recursive clear
					(std::static_pointer_cast<ShaderFunction>(m_pInput[i]->GetOutputLink()->GetOwner()))->ClearShaderTreeStringFlag();
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
		Renderer3D::ValueElement Mask[4];
		Mask[0] = Renderer3D::ValueElement::VE_R;
		Mask[1] = Renderer3D::ValueElement::VE_G;
		Mask[2] = Renderer3D::ValueElement::VE_B;
		Mask[3] = Renderer3D::ValueElement::VE_A;

		for (uint32_t i = 0; i < 4; ++i)
		{
			if (i > pOutputNode->GetValueType())
			{
				Value[i] = Renderer3D::GetValueElement(pOutputNode, Mask[pOutputNode->GetValueType()]);
			}
			else
			{
				Value[i] = Renderer3D::GetValueElement(pOutputNode, Mask[i]);
			}
		}

		//similar to vec4(xx.r, xx.r, xx.r, xx.r);
		if (pInputNode->GetValueType() == PutNode::VT_1)
		{
			OutString += Renderer3D::FloatConst(Value[0]);
		}
		else if (pInputNode->GetValueType() == PutNode::VT_2)
		{
			OutString += Renderer3D::FloatConst2(Value[0], Value[1]);
		}
		else if (pInputNode->GetValueType() == PutNode::VT_3)
		{
			OutString += Renderer3D::FloatConst3(Value[0], Value[1], Value[2]);
		}
		else if(pInputNode->GetValueType() == PutNode::VT_4)
		{
			OutString += Renderer3D::FloatConst4(Value[0], Value[1], Value[2], Value[3]);
		}
		else
		{
			return "";
		}
		OutString += ";\n";
		return OutString;
	}

}