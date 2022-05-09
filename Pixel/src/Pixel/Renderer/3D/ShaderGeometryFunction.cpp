#include "pxpch.h"
#include "ShaderGeometryFunction.h"
#include "Pixel/Renderer/3D/Renderer3D.h"
#include "Pixel/Renderer/3D/ShaderStringFactory.h"

namespace Pixel {

	ShaderGeometryFunction::ShaderGeometryFunction()
	{
		m_functionType = ShaderFunctionType::GeoMetry;
	}

	//because shared_from_this need object have constructed
	void ShaderGeometryFunction::Init()
	{
		std::string InputName = "Pos";
		Ref<InputNode> pInputNode = CreateRef<InputNode>(PutNode::VT_4, InputName, shared_from_this());
		m_pInput.push_back(pInputNode);

		InputName = "Albedo";
		pInputNode = CreateRef<InputNode>(PutNode::VT_4, InputName, shared_from_this());
		m_pInput.push_back(pInputNode);

		InputName = "Normal";
		pInputNode = CreateRef<InputNode>(PutNode::VT_4, InputName, shared_from_this());
		m_pInput.push_back(pInputNode);

		InputName = "Roughness";
		pInputNode = CreateRef<InputNode>(PutNode::VT_1, InputName, shared_from_this());
		m_pInput.push_back(pInputNode);

		InputName = "Metallic";
		pInputNode = CreateRef<InputNode>(PutNode::VT_1, InputName, shared_from_this());
		m_pInput.push_back(pInputNode);

		InputName = "Emissive";
		pInputNode = CreateRef<InputNode>(PutNode::VT_1, InputName, shared_from_this());
		m_pInput.push_back(pInputNode);

		std::string OutputName = "O_WorldPos";
		Ref<OutputNode> pOutputNode = CreateRef<OutputNode>(PutNode::VT_4, OutputName, shared_from_this());
		m_pOutput.push_back(pOutputNode);

		OutputName = "O_Albedo";
		pOutputNode = CreateRef<OutputNode>(PutNode::VT_4, OutputName, shared_from_this());
		m_pOutput.push_back(pOutputNode);

		OutputName = "O_Normal";
		pOutputNode = CreateRef<OutputNode>(PutNode::VT_4, OutputName, shared_from_this());
		m_pOutput.push_back(pOutputNode);

		OutputName = "O_Roughness";
		pOutputNode = CreateRef<OutputNode>(PutNode::VT_1, OutputName, shared_from_this());
		m_pOutput.push_back(pOutputNode);

		OutputName = "O_Metallic";
		pOutputNode = CreateRef<OutputNode>(PutNode::VT_1, OutputName, shared_from_this());
		m_pOutput.push_back(pOutputNode);

		OutputName = "O_Emissive";
		pOutputNode = CreateRef<OutputNode>(PutNode::VT_1, OutputName, shared_from_this());
		m_pOutput.push_back(pOutputNode);
	}

	ShaderGeometryFunction::~ShaderGeometryFunction()
	{

	}

	bool ShaderGeometryFunction::GetFunctionString(std::string& OutString) const
	{
		if (m_pInput[IN_POS]->GetOutputLink() == nullptr)
		{
			//fix: temp to fix up the hard code v_Pos
			OutString += m_pOutput[OUT_POS]->GetNodeName() + " = " +
			"vec4(v_Pos, 1.0)" + ";\n";
		}
		else
		{
			//there calculate light and shadow
			Ref<OutputNode> pOutputNode = m_pInput[IN_POS]->GetOutputLink();
			OutString += m_pOutput[OUT_POS]->GetNodeName() + " = " +
				pOutputNode->GetNodeName() + ";\n";
		}

		if (m_pInput[IN_ALBEDO]->GetOutputLink() == nullptr)
		{
			OutString += m_pOutput[OUT_ALBEDO]->GetNodeName() + " = " +
				Renderer3D::FloatConst4("0", "0", "0", "0") + ";\n";
		}
		else
		{
			//there calculate light and shadow
			Ref<OutputNode> pOutputNode = m_pInput[IN_ALBEDO]->GetOutputLink();
			OutString += m_pOutput[OUT_ALBEDO]->GetNodeName() + " = " +
				pOutputNode->GetNodeName() + ";\n";
		}

		if (m_pInput[IN_NORMAL]->GetOutputLink() == nullptr)
		{
			OutString += m_pOutput[OUT_NORMAL]->GetNodeName() + " = " +
				Renderer3D::FloatConst4("0", "0", "0", "0") + ";\n";
		}
		else
		{
			//there calculate light and shadow
			Ref<OutputNode> pOutputNode = m_pInput[IN_NORMAL]->GetOutputLink();
			OutString += m_pOutput[OUT_NORMAL]->GetNodeName() + " = " +
				pOutputNode->GetNodeName() + ";\n";
		}

		if (m_pInput[IN_ROUGHNESS]->GetOutputLink() == nullptr)
		{
			OutString += m_pOutput[OUT_ROUGHNESS]->GetNodeName() + " = " +
				Renderer3D::FloatConst("0") + ";\n";
		}
		else
		{
			//there calculate light and shadow
			Ref<OutputNode> pOutputNode = m_pInput[IN_ROUGHNESS]->GetOutputLink();
			OutString += m_pOutput[OUT_ROUGHNESS]->GetNodeName() + " = " +
				pOutputNode->GetNodeName() + ";\n";
		}

		if (m_pInput[IN_METALLIC]->GetOutputLink() == nullptr)
		{
			OutString += m_pOutput[OUT_METALLIC]->GetNodeName() + " = " +
				Renderer3D::FloatConst("0") + ";\n";
		}
		else
		{
			//there calculate light and shadow
			Ref<OutputNode> pOutputNode = m_pInput[IN_METALLIC]->GetOutputLink();
			OutString += m_pOutput[OUT_METALLIC]->GetNodeName() + " = " +
				pOutputNode->GetNodeName() + ";\n";
		}

		if (m_pInput[IN_EMISSIVE]->GetOutputLink() == nullptr)
		{
			OutString += m_pOutput[OUT_EMISSIVE]->GetNodeName() + " = " +
				Renderer3D::FloatConst("0") + ";\n";
		}
		else
		{
			//there calculate light and shadow
			Ref<OutputNode> pOutputNode = m_pInput[IN_EMISSIVE]->GetOutputLink();
			OutString += m_pOutput[OUT_EMISSIVE]->GetNodeName() + " = " +
				pOutputNode->GetNodeName() + ";\n";
		}

		//m_PSOutputColorValue, pixel shader's output node name
		//m_PSOutputColorValue is initialized at ShaderStringFactory Init Function
		OutString += "g_Position = " + m_pOutput[OUT_POS]->GetNodeName() + ".xyz" + ";\n";
		OutString += "g_Normal = " + m_pOutput[OUT_NORMAL]->GetNodeName() + ".xyz" + ";\n";
		OutString += "g_Albedo = " + m_pOutput[OUT_ALBEDO]->GetNodeName() + ".xyz" + ";\n";
		OutString += "g_RoughnessMetallicEmissive.x = " + m_pOutput[OUT_ROUGHNESS]->GetNodeName() + ";\n";
		OutString += "g_RoughnessMetallicEmissive.y = " + m_pOutput[OUT_METALLIC]->GetNodeName() + ";\n";
		OutString += "g_RoughnessMetallicEmissive.z = " + m_pOutput[OUT_EMISSIVE]->GetNodeName() + ";\n";
		
		return true;
	}

}