#include "pxpch.h"
#include "ShaderPbrFunction.h"
#include "Pixel/Renderer/3D/Renderer3D.h"
#include "Pixel/Renderer/3D/ShaderStringFactory.h"

namespace Pixel {

	ShaderPbrFunction::ShaderPbrFunction()
	{
		
	}

	//because shared_from_this need object have constructed
	void ShaderPbrFunction::Init()
	{
		std::string InputName = "Albedo";
		Ref<InputNode> pInputNode = CreateRef<InputNode>(PutNode::VT_4, InputName, shared_from_this());
		m_pInput.push_back(pInputNode);

		InputName = "Roughness";
		pInputNode = CreateRef<InputNode>(PutNode::VT_1, InputName, shared_from_this());
		m_pInput.push_back(pInputNode);

		InputName = "Metallic";
		pInputNode = CreateRef<InputNode>(PutNode::VT_1, InputName, shared_from_this());
		m_pInput.push_back(pInputNode);

		InputName = "Normal";
		pInputNode = CreateRef<InputNode>(PutNode::VT_3, InputName, shared_from_this());
		m_pInput.push_back(pInputNode);

		std::string OutputName = "Color";
		Ref<OutputNode> pOutputNode = CreateRef<OutputNode>(PutNode::VT_4, OutputName, shared_from_this());
		m_pOutput.push_back(pOutputNode);
	}

	ShaderPbrFunction::~ShaderPbrFunction()
	{

	}

	bool ShaderPbrFunction::GetFunctionString(std::string& OutString) const
	{
		if (m_pInput[IN_ALBEDO]->GetOutputLink() == nullptr)
		{
			OutString += m_pOutput[OUT_COLOR]->GetNodeName() + " = " +
				Renderer3D::FloatConst4("0", "0", "0", "1") + ";\n";
		}
		else
		{
			//there calculate light and shadow
			Ref<OutputNode> pOutputNode = m_pInput[IN_ALBEDO]->GetOutputLink();
			OutString += m_pInput[OUT_COLOR]->GetNodeName() + " = " +
				pOutputNode->GetNodeName() + ";\n";
		}
		//m_PSOutputColorValue, pixel shader's output node name
		//m_PSOutputColorValue is initialized at ShaderStringFactory Init Function
		OutString += ShaderStringFactory::m_PSOutputColorValue + " = " + m_pOutput[OUT_COLOR]->GetNodeName() + ";\n";

		return true;
	}

}