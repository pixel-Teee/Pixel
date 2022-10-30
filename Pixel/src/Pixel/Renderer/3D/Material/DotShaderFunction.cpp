#include "pxpch.h"

#include "DotShaderFunction.h"
#include "ShaderFunction.h"
#include "ShaderStringFactory.h"
#include "InputNode.h"
#include "OutputNode.h"

namespace Pixel {
	DotShaderFunction::DotShaderFunction()
	{
		m_InputNodeDisplayColor = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
		m_InputNodeDisplayName = { "A", "B" };

		m_OutputNodeDisplayColor = { glm::vec3(1.0f, 1.0f, 1.0f) };
		m_OutputNodeDisplayName = { "" };

		m_HeaderColor = glm::vec3(0.75f, 0.82f, 0.93f);
	}

	DotShaderFunction::DotShaderFunction(const std::string& showName, Ref<Material> pMaterial) : ShaderFunction(showName, pMaterial)
	{
		m_InputNodeDisplayColor = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
		m_InputNodeDisplayName = { "A", "B" };

		m_OutputNodeDisplayColor = { glm::vec3(1.0f, 1.0f, 1.0f) };
		m_OutputNodeDisplayName = { "" };

		m_HeaderColor = glm::vec3(0.75f, 0.82f, 0.93f);
	}

	DotShaderFunction::~DotShaderFunction()
	{

	}

	bool DotShaderFunction::GetFunctionString(std::string& OutString) const
	{
		OutString += m_pOutputs[0]->GetNodeName() + " = " + "dot(" + m_pInputs[0]->GetNodeName() + ", " + m_pInputs[1]->GetNodeName() + ");\n";
		return true;
	}

	void DotShaderFunction::ConstructPutNodeAndSetPutNodeOwner()
	{
		ShaderFunction::ConstructPutNodeAndSetPutNodeOwner();

		//default is vt_4
		std::string InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string InputName = "DotA" + InputId;
		Ref<InputNode> pInputNode = nullptr;
		pInputNode = CreateRef<InputNode>(ValueType::VT_4, InputName, shared_from_this());
		m_pInputs.push_back(pInputNode);
		//m_InputNodeDisplayName.push_back("A");
		//m_InputNodeDisplayColor.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

		InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		InputName = "DotB" + InputId;
		pInputNode = nullptr;
		pInputNode = CreateRef<InputNode>(ValueType::VT_4, InputName, shared_from_this());
		m_pInputs.push_back(pInputNode);
		//m_InputNodeDisplayName.push_back("B");
		//m_InputNodeDisplayColor.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

		std::string OutputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "Scalar" + OutputId;
		Ref<OutputNode> pOutputNode = nullptr;
		pOutputNode = CreateRef<OutputNode>(ValueType::VT_4, OutputName, shared_from_this());
		m_pOutputs.push_back(pOutputNode);
		//m_OutputNodeDisplayName.push_back("");
		//m_OutputNodeDisplayColor.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	}

	bool DotShaderFunction::ResetValueType() const
	{
		uint32_t minType = (uint32_t)ValueType::VT_4;

		for (uint32_t i = 0; i < m_pInputs.size(); ++i)
		{
			if (m_pInputs[i]->GetOutputLink())
			{
				if (minType > (uint32_t)m_pInputs[i]->GetOutputLink()->GetValueType())
				{
					minType = (uint32_t)m_pInputs[i]->GetOutputLink()->GetValueType();
				}
			}
		}

		for (uint32_t i = 0; i < m_pInputs.size(); ++i)
		{
			m_pInputs[i]->SetValueType(static_cast<ValueType>(minType));
		}

		//m_pOutputs[0]->SetValueType(static_cast<ValueType>(MaxType));

		/*
		uint32_t MaxType = (uint32_t)ValueType::VT_1;

		for (uint32_t i = 0; i < m_pInputs.size(); ++i)
		{
			if (m_pInputs[i]->GetOutputLink())
			{
				if (MaxType < (uint32_t)m_pInputs[i]->GetOutputLink()->GetValueType())
				{
					MaxType = (uint32_t)m_pInputs[i]->GetOutputLink()->GetValueType();
				}
			}
		}

		for (uint32_t i = 0; i < m_pInputs.size(); ++i)
		{
			m_pInputs[i]->SetValueType(static_cast<ValueType>(MaxType));
		}

		m_pOutputs[0]->SetValueType(static_cast<ValueType>(MaxType));
		*/
		return true;
	}
}