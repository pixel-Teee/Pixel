#include "pxpch.h"
#include "ShaderFunction.h"
#include "Renderer3D.h"
#include "ShaderStringFactory.h"
#include "Material.h"

namespace Pixel {
	class Material;

	ShaderFunction::ShaderFunction(const std::string& ShowName, Ref<Material> pMaterial)
	{
		m_bIsVisited = false;
		m_ShowName = ShowName;
		m_pInput.clear();
		m_pOutput.clear();
		m_pOwner = pMaterial;
	}

	ShaderFunction::~ShaderFunction()
	{
		m_pOwner->DeleteShaderFunction(std::shared_ptr<ShaderFunction>(shared_from_this()));
	}

	void ShaderFunction::AddToMaterialOwner()
	{
		m_pOwner->AddShaderFunction(std::shared_ptr<ShaderFunction>(shared_from_this()));
	}

	void ShaderFunction::ConstrcutPutNodeAndSetPutNodeOwner()
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
		return m_pInput[nodeId];
	}

	Ref<InputNode> ShaderFunction::GetInputNode(const std::string& NodeName) const
	{
		for (uint32_t i = 0; i < m_pInput.size(); ++i)
		{
			if (m_pInput[i]->GetNodeName() == NodeName)
				return m_pInput[i];
		}
		return nullptr;
	}

	Ref<OutputNode> ShaderFunction::GetOutputNode(uint32_t nodeId) const
	{
		return m_pOutput[nodeId];
	}

	Ref<OutputNode> ShaderFunction::GetOutputNode(const std::string& NodeName) const
	{
		for (uint32_t i = 0; i < m_pOutput.size(); ++i)
		{
			if (m_pOutput[i]->GetNodeName() == NodeName)
				return m_pOutput[i];
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

	//------ConstValue------
	ConstValue::ConstValue(const std::string& showName, Ref<Material> pMaterial, uint32_t valueNumber, bool bIsCustom)
		:ShaderFunction(showName, pMaterial)
	{
		m_bIsCustom = bIsCustom;
		m_functionType = ShaderFunction::ConstFloat4;
	}

	ConstValue::~ConstValue()
	{

	}
	//------ConstValue------

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
		m_pOutput.push_back(pOutputNode);

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
			std::string OutputNameR = Renderer3D::GetValueElement(GetOutputNode(OUT_VALUE), Renderer3D::VE_R);//PutNode name just ConstFloatValueID.x
			Ref<OutputNode> pOutputNode;
			pOutputNode = CreateRef<OutputNode>(PutNode::ValueType::VT_1, OutputNameR, shared_from_this());
			m_pOutput.push_back(pOutputNode);
		}

		if (m_valueNumber >= 2)
		{
			std::string OutputNameG = Renderer3D::GetValueElement(GetOutputNode(OUT_VALUE), Renderer3D::VE_G);
			Ref<OutputNode> pOutputNode;
			pOutputNode = CreateRef<OutputNode>(PutNode::ValueType::VT_1, OutputNameG, shared_from_this());
			m_pOutput.push_back(pOutputNode);
		}

		if (m_valueNumber >= 3)
		{
			std::string OutputNameB = Renderer3D::GetValueElement(GetOutputNode(OUT_VALUE), Renderer3D::VE_B);
			Ref<OutputNode> pOutputNode;
			pOutputNode = CreateRef<OutputNode>(PutNode::ValueType::VT_1, OutputNameB, shared_from_this());
			m_pOutput.push_back(pOutputNode);
		}

		if (m_valueNumber >= 4)
		{
			std::string OutputNameA = Renderer3D::GetValueElement(GetOutputNode(OUT_VALUE), Renderer3D::VE_A);
			Ref<OutputNode> pOutputNode;
			pOutputNode = CreateRef<OutputNode>(PutNode::ValueType::VT_1, OutputNameA, shared_from_this());
			m_pOutput.push_back(pOutputNode);
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
		if (m_pOutput[0]->GetValueType() == CustomFloatValue::VT_1)
		{
			OutString += Renderer3D::Float() + " ";
		}
		else if (m_pOutput[0]->GetValueType() == CustomFloatValue::VT_2)
		{
			OutString += Renderer3D::Float2() + " ";
		}
		else if (m_pOutput[0]->GetValueType() == CustomFloatValue::VT_3)
		{
			OutString += Renderer3D::Float3() += " ";
		}
		else if (m_pOutput[0]->GetValueType() == CustomFloatValue::VT_4)
		{
			OutString += Renderer3D::Float4() += " ";
		}

		OutString += m_pOutput[0]->GetNodeName() += ";\n";
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
		OutString += m_pOutput[0]->GetNodeName() + " = ";
		if (m_pOutput[0]->GetValueType() == PutNode::VT_1)
		{
			OutString += Renderer3D::FloatConst(Value[0]);
		}
		else if (m_pOutput[0]->GetValueType() == PutNode::VT_2)
		{
			OutString += Renderer3D::FloatConst2(Value[0], Value[1]);
		}
		else if (m_pOutput[0]->GetValueType() == PutNode::VT_3)
		{
			OutString += Renderer3D::FloatConst3(Value[0], Value[1], Value[2]);
		}
		else if (m_pOutput[0]->GetValueType() == PutNode::VT_4)
		{
			OutString += Renderer3D::FloatConst4(Value[0], Value[1], Value[2], Value[3]);
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
		if (m_pOutput[0]->GetValueType() == PutNode::VT_1)
		{
			OutString += Renderer3D::Float() + " ";
		}
		else if (m_pOutput[1]->GetValueType() == PutNode::VT_2)
		{
			OutString += Renderer3D::Float2() + " ";
		}
		else if (m_pOutput[2]->GetValueType() == PutNode::VT_3)
		{
			OutString += Renderer3D::Float3() + " ";
		}
		else if (m_pOutput[3]->GetValueType() == PutNode::VT_4)
		{
			OutString += Renderer3D::Float4() += " ";
		}
		else
			return false;

		//uniform vec4 xx;
		OutString += Renderer3D::Uniform() + m_pOutput[0]->GetNodeName() + (";\n");
		return true;
	}

	void ConstFloatValue::ResetInShaderName()
	{
		//reset name
		std::string OutputId = std::to_string(ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "ConstFloatValue" + OutputId;
		m_pOutput[0]->SetNodeName(OutputName);
		++ShaderStringFactory::m_ShaderValueIndex;
		if (m_Value.size() >= 1)
		{
			std::string OutputNameR = Renderer3D::GetValueElement(GetOutputNode(OUT_VALUE), Renderer3D::VE_R);
			Ref<OutputNode> pOutputNode;
			m_pOutput[0]->SetNodeName(OutputNameR);
		}

		if (m_Value.size() >= 2)
		{
			std::string OutputNameG = Renderer3D::GetValueElement(GetOutputNode(OUT_VALUE), Renderer3D::VE_G);
			Ref<OutputNode> pOutputNode;
			pOutputNode = CreateRef<OutputNode>(PutNode::ValueType::VT_1, OutputNameG, std::shared_ptr<ShaderFunction>(shared_from_this()));
			m_pOutput[0]->SetNodeName(OutputNameG);
		}

		if (m_Value.size() >= 3)
		{
			std::string OutputNameB = Renderer3D::GetValueElement(GetOutputNode(OUT_VALUE), Renderer3D::VE_B);
			Ref<OutputNode> pOutputNode;
			pOutputNode = CreateRef<OutputNode>(PutNode::ValueType::VT_1, OutputNameB, std::shared_ptr<ShaderFunction>(shared_from_this()));
			m_pOutput[0]->SetNodeName(OutputNameB);
		}

		if (m_Value.size() >= 4)
		{
			std::string OutputNameA = Renderer3D::GetValueElement(GetOutputNode(OUT_VALUE), Renderer3D::VE_A);
			Ref<OutputNode> pOutputNode;
			pOutputNode = CreateRef<OutputNode>(PutNode::ValueType::VT_1, OutputNameA, std::shared_ptr<ShaderFunction>(shared_from_this()));
			m_pOutput[0]->SetNodeName(OutputNameA);
		}
	}
	//------ConstFloatValue------

	bool ShaderMainFunction::GetInputValueString(std::string& OutString, uint32_t uiOutPutStringType)
	{
		std::string Temp;
		if (uiOutPutStringType == OST_MATERIAL)
		{
			for (uint32_t i = 0; i < m_pInput.size(); ++i)
			{
				if (m_pInput[i] == GetNormalNode())
					continue;
				if (m_pInput[i]->GetValueType() == PutNode::VT_1)
				{
					OutString += Renderer3D::Float() + " ";
					
					Temp = Renderer3D::FloatConst("0");
				}
				else if (m_pInput[i]->GetValueType() == PutNode::VT_2)
				{
					OutString += Renderer3D::Float2() + " ";

					Temp = Renderer3D::FloatConst2("0", "0");
				}
				else if (m_pInput[i]->GetValueType() == PutNode::VT_3)
				{
					OutString += Renderer3D::Float3() + " ";

					Temp = Renderer3D::FloatConst3("0", "0", "0");
				}
				else if (m_pInput[i]->GetValueType() == PutNode::VT_4)
				{
					OutString += Renderer3D::Float4() + " ";

					Temp = Renderer3D::FloatConst4("0", "0", "0", "0");
				}

				if (!m_pInput[i]->GetOutputLink())
				{
					OutString += m_pInput[i]->GetNodeName() + " = " + Temp + ";\n";
					continue;
				}
				
				OutString += GetValueEqualString(m_pInput[i]->GetOutputLink(), m_pInput[i]);
			}			
		}

		return true;
	}

	//------Declare nonuser custom variable------
	//------float3 WorldPos------
	//------float3 WorldNormal------
	void ShaderMainFunction::GetValueUseDeclareString(std::string& OutString, uint32_t uiValueUseString)
	{
		std::string DefaultValue = Renderer3D::FloatConst3("0", "0", "0");
		if ((uiValueUseString & VUS_WORLD_POS) == VUS_WORLD_POS)
		{
			OutString += Renderer3D::Float3() + ShaderStringFactory::m_WorldPos + " = " + DefaultValue + ";\n";
		}

		if ((uiValueUseString & VUS_WORLD_NORMAL) == VUS_WORLD_NORMAL)
		{
			OutString += Renderer3D::Float3() + ShaderStringFactory::m_WorldNormal + " = " + DefaultValue + ";\n";
		}
	}
	//------Declare nonuser custom variable------

	bool ShaderMainFunction::GetShaderTreeString(std::string& OutString, uint32_t uiOutPutStringType)
	{
		if (m_bIsVisited == true)
			return true;
		else
		{
			m_bIsVisited = true;
			if (uiOutPutStringType == OST_MATERIAL)
			{
				//declare nonuser custom variable
				//WorldPos¡¢WorldNormal etc.
				GetValueUseDeclareString(OutString, VUS_ALL);

				//get main function input node's input code
				//becase WorldNormal need this, and roughness and metallic are also need this
				GetNormalString(OutString);

				GetValueUseString(OutString, VUS_ALL);

				for (uint32_t i = 0; i < m_pInput.size(); ++i)
				{
					if (m_pInput[i]->GetOutputLink() == nullptr)
						continue;
					else if (m_pInput[i] == GetNormalNode())
					{
						continue;
					}
					else
					{
						(std::static_pointer_cast<ShaderFunction>(m_pInput[i]->GetOutputLink()->GetOwner()))->GetShaderTreeString(OutString);
					}
				}
			}
		}

		//input temp variable declare
		if (!GetInputValueString(OutString, uiOutPutStringType))
			return false;

		//output temp variable declare
		if (!GetOutputValueString(OutString))
			return false;

		//calculate output temp variable from input temp variable
		if (uiOutPutStringType == OST_MATERIAL)
		{
			if (!GetFunctionString(OutString))
				return false;
		}
		return true;
	}

	void ShaderMainFunction::GetValueUseString(std::string& OutString, uint32_t uiOutPutStringType)
	{
		if ((uiOutPutStringType & VUS_WORLD_NORMAL) == VUS_WORLD_NORMAL)
		{
			if (ShaderStringFactory::m_PSInputLocalNormal.size() == 0)
			{
				OutString += ShaderStringFactory::m_WorldNormal + " = " +
					Renderer3D::FloatConst3("0", "0", "0") + ";\n";
			}
		}

		if ((uiOutPutStringType & VUS_WORLD_POS) == VUS_WORLD_POS)
		{
			//OutString += "	" + ShaderStringFactory::m_WorldPos + " = ";
			//Local World To Pos
			//ShaderStringFactory::LocalToWorldPos(ShaderStringFactory::m_PSInputLocalNormal, OutString);
			//OutString += ";\n";
		}
	}

	void ShaderMainFunction::GetNormalString(std::string& OutString)
	{
		if (GetNormalNode()->GetOutputLink())
		{
			(std::static_pointer_cast<ShaderFunction>(GetNormalNode()->GetOutputLink()->GetOwner()))->GetShaderTreeString(OutString);
		}
	}

}