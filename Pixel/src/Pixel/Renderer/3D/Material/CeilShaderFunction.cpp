#include "pxpch.h"

#include "CeilShaderFunction.h"
#include "ShaderFunction.h"
#include "ShaderStringFactory.h"
#include "InputNode.h"
#include "OutputNode.h"

namespace Pixel {
	CeilShaderFunction::CeilShaderFunction()
	{
		//m_functionType = ShaderFunctionType::Mul;
		m_InputNodeDisplayColor = { glm::vec3(1.0f, 0.0f, 0.0f) };
		m_InputNodeDisplayName = { "x" };

		m_OutputNodeDisplayColor = { glm::vec3(1.0f, 1.0f, 1.0f) };
		m_OutputNodeDisplayName = { "" };

		m_HeaderColor = glm::vec3(0.75f, 0.82f, 0.93f);
	}

	CeilShaderFunction::CeilShaderFunction(const std::string& showName, Ref<Material> pMaterial) : ShaderFunction(showName, pMaterial)
	{
		//m_functionType = ShaderFunctionType::Mul;
		m_InputNodeDisplayColor = { glm::vec3(1.0f, 0.0f, 0.0f) };
		m_InputNodeDisplayName = { "x" };

		m_OutputNodeDisplayColor = { glm::vec3(1.0f, 1.0f, 1.0f) };
		m_OutputNodeDisplayName = { "" };

		m_HeaderColor = glm::vec3(0.75f, 0.82f, 0.93f);
	}


	CeilShaderFunction::~CeilShaderFunction()
	{

	}


	bool CeilShaderFunction::GetFunctionString(std::string& OutString) const
	{
		OutString += m_pOutputs[0]->GetNodeName() + " = ceil(" + m_pInputs[0]->GetNodeName() + ");\n";
		return true;
	}


	bool CeilShaderFunction::ResetValueType() const
	{
		uint32_t CurrentType = (uint32_t)ValueType::VT_4;

		for (uint32_t i = 0; i < m_pInputs.size(); ++i)
		{
			if (m_pInputs[i]->GetOutputLink())
			{
				/*
				if (MaxType < (uint32_t)m_pInputs[i]->GetOutputLink()->GetValueType())
				{
					MaxType = (uint32_t)m_pInputs[i]->GetOutputLink()->GetValueType();
				}
				*/
				CurrentType = i;
				break;
			}
		}

		for (uint32_t i = 0; i < m_pInputs.size(); ++i)
		{
			m_pInputs[i]->SetValueType(static_cast<ValueType>(CurrentType));
		}

		m_pOutputs[0]->SetValueType(static_cast<ValueType>(CurrentType));

		return true;
	}

	void CeilShaderFunction::ConstructPutNodeAndSetPutNodeOwner()
	{
		ShaderFunction::ConstructPutNodeAndSetPutNodeOwner();

		//default is vt_4
		std::string InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string InputName = "InputValue" + InputId;
		Ref<InputNode> pInputNode = nullptr;
		pInputNode = CreateRef<InputNode>(ValueType::VT_4, InputName, shared_from_this());
		m_pInputs.push_back(pInputNode);

		std::string OutputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "CeilOutput" + OutputId;
		Ref<OutputNode> pOutputNode = nullptr;
		pOutputNode = CreateRef<OutputNode>(ValueType::VT_4, OutputName, shared_from_this());
		m_pOutputs.push_back(pOutputNode);
	}

}