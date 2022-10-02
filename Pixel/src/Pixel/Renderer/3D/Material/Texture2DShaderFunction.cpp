#include "pxpch.h"

#include "Texture2DShaderFunction.h"
#include "ShaderStringFactory.h"
#include "InputNode.h"
#include "OutputNode.h"

namespace Pixel {
	Texture2DShaderFunction::Texture2DShaderFunction()
	{
		m_HeaderColor = { 0.11f, 0.56f, 1.0f };
	}

	Texture2DShaderFunction::Texture2DShaderFunction(const std::string& ShowName, Ref<Material> pMaterial)
		: TextureShaderFunction(ShowName, pMaterial)
	{
		m_HeaderColor = { 0.11f, 0.56f, 1.0f };
	}

	void Texture2DShaderFunction::ConstructPutNodeAndSetPutNodeOwner()
	{
		//input
		std::string InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string InputName = "Texture2DCoordinateInput" + InputId;
		Ref<InputNode> pInputNode = CreateRef<InputNode>(ValueType::VT_2, InputName, shared_from_this());
		m_pInputs.push_back(pInputNode);
		m_InputNodeDisplayColor.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
		m_InputNodeDisplayName.push_back("UV");

		//output
		std::string OutputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "Texture2DOutput" + OutputId;
		Ref<OutputNode> pOutputNode = CreateRef<OutputNode>(ValueType::VT_4, OutputName, shared_from_this());
		m_pOutputs.push_back(pOutputNode);
		m_OutputNodeDisplayColor.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
		m_OutputNodeDisplayName.push_back("");
		//r 
		std::string OutputNameR = ShaderStringFactory::GetValueElement(pOutputNode, ShaderStringFactory::VE_R);
		pOutputNode = CreateRef<OutputNode>(ValueType::VT_1, OutputNameR, shared_from_this());
		m_pOutputs.push_back(pOutputNode);
		m_OutputNodeDisplayColor.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		m_OutputNodeDisplayName.push_back("");
		//g
		std::string OutputNameG = ShaderStringFactory::GetValueElement(pOutputNode, ShaderStringFactory::VE_G);
		pOutputNode = CreateRef<OutputNode>(ValueType::VT_1, OutputNameG, shared_from_this());
		m_pOutputs.push_back(pOutputNode);
		m_OutputNodeDisplayColor.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
		m_OutputNodeDisplayName.push_back("");
		//b
		std::string OutputNameB = ShaderStringFactory::GetValueElement(pOutputNode, ShaderStringFactory::VE_B);
		pOutputNode = CreateRef<OutputNode>(ValueType::VT_1, OutputNameB, shared_from_this());
		m_pOutputs.push_back(pOutputNode);
		m_OutputNodeDisplayColor.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
		m_OutputNodeDisplayName.push_back("");
		//a
		std::string OutputNameA = ShaderStringFactory::GetValueElement(pOutputNode, ShaderStringFactory::VE_A);
		pOutputNode = CreateRef<OutputNode>(ValueType::VT_1, OutputNameA, shared_from_this());
		m_pOutputs.push_back(pOutputNode);
		m_OutputNodeDisplayColor.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
		m_OutputNodeDisplayName.push_back("");
	}

	Texture2DShaderFunction::~Texture2DShaderFunction()
	{

	}

	bool Texture2DShaderFunction::GetFunctionString(std::string& OutString) const
	{
		OutString += ShaderStringFactory::Tex2D(const_cast<Texture2DShaderFunction*>(this));

		//TODO:if it is normal map, need to decode

		return true;
	}

	void Texture2DShaderFunction::ResetInShaderName()
	{
		std::string InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string InputName = "Texture2DCoordinateInput" + InputId;
		m_pInputs[0]->SetNodeName(InputName);
		
		std::string OutputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "Texture2DOutput" + OutputId;
		m_pOutputs[0]->SetNodeName(OutputName);

		std::string OutputNameR = ShaderStringFactory::GetValueElement(GetOutputNode(OUT_COLOR), ShaderStringFactory::VE_R);
		m_pOutputs[1]->SetNodeName(OutputNameR);

		std::string OutputNameG = ShaderStringFactory::GetValueElement(GetOutputNode(OUT_COLOR), ShaderStringFactory::VE_G);
		m_pOutputs[2]->SetNodeName(OutputNameG);

		std::string OutputNameB = ShaderStringFactory::GetValueElement(GetOutputNode(OUT_COLOR), ShaderStringFactory::VE_B);
		m_pOutputs[3]->SetNodeName(OutputNameB);

		std::string OutputNameA = ShaderStringFactory::GetValueElement(GetOutputNode(OUT_COLOR), ShaderStringFactory::VE_A);
		m_pOutputs[4]->SetNodeName(OutputNameA);
	}

	bool Texture2DShaderFunction::GetOutputValueString(std::string& OutString) const
	{
		std::string Temp;
		if (m_pOutputs[0]->GetValueType() == ValueType::VT_1)
		{
			OutString += ShaderStringFactory::Float() + " ";
			Temp = ShaderStringFactory::FloatConst("0");
		}
		else if (m_pOutputs[0]->GetValueType() == ValueType::VT_2)
		{
			OutString += ShaderStringFactory::Float2() + " ";
			Temp = ShaderStringFactory::FloatConst2("0", "0");
		}
		else if (m_pOutputs[0]->GetValueType() == ValueType::VT_3)
		{
			OutString += ShaderStringFactory::Float3() + " ";
			Temp = ShaderStringFactory::FloatConst3("0", "0", "0");
		}
		else if (m_pOutputs[0]->GetValueType() == ValueType::VT_4)
		{
			OutString += ShaderStringFactory::Float4() + " ";
			Temp = ShaderStringFactory::FloatConst4("0", "0", "0", "0");
		}
		else
			return false;

		OutString += m_pOutputs[0]->GetNodeName() + " = " + Temp + ";\n";

		return true;
	}
}