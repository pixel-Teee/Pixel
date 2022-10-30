#include "pxpch.h"

#include "SinShaderFunction.h"
#include "ShaderFunction.h"
#include "ShaderStringFactory.h"
#include "InputNode.h"
#include "OutputNode.h"

namespace Pixel {
	SinShaderFunction::SinShaderFunction()
	{
		//m_functionType = ShaderFunctionType::Mul;
		m_InputNodeDisplayColor = { glm::vec3(1.0f, 0.0f, 0.0f) };
		m_InputNodeDisplayName = { "x" };

		m_OutputNodeDisplayColor = { glm::vec3(1.0f, 1.0f, 1.0f) };
		m_OutputNodeDisplayName = { "" };

		m_HeaderColor = glm::vec3(0.75f, 0.82f, 0.93f);
	}

	SinShaderFunction::SinShaderFunction(const std::string& showName, Ref<Material> pMaterial)
	: ShaderFunction(showName, pMaterial)
	{
		//m_functionType = ShaderFunctionType::Mul;
		m_InputNodeDisplayColor = { glm::vec3(1.0f, 0.0f, 0.0f) };
		m_InputNodeDisplayName = { "x" };

		m_OutputNodeDisplayColor = { glm::vec3(1.0f, 1.0f, 1.0f) };
		m_OutputNodeDisplayName = { "" };

		m_HeaderColor = glm::vec3(0.75f, 0.82f, 0.93f);
	}

	SinShaderFunction::~SinShaderFunction()
	{

	}

	bool SinShaderFunction::GetFunctionString(std::string& OutString) const
	{
		//use built-in function
		OutString += m_pOutputs[0]->GetNodeName() + " = " + "sin(" + m_pInputs[0]->GetNodeName() + ");\n";
		return true;
	}

	/*
	bool SinShaderFunction::ResetValueType() const
	{

	}
	*/

	void SinShaderFunction::ConstructPutNodeAndSetPutNodeOwner()
	{
		ShaderFunction::ConstructPutNodeAndSetPutNodeOwner();

		std::string InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string InputName = "SinInputValue" + InputId;
		Ref<InputNode> pInputNode = nullptr;
		pInputNode = CreateRef<InputNode>(ValueType::VT_4, InputName, shared_from_this());
		m_pInputs.push_back(pInputNode);

		std::string OutputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "SinOutputValue" + OutputId;
		Ref<OutputNode> pOutputNode = nullptr;
		pOutputNode = CreateRef<OutputNode>(ValueType::VT_4, OutputName, shared_from_this());
		m_pOutputs.push_back(pOutputNode);
	}
}