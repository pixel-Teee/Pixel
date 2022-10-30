#include "pxpch.h"

#include "LerpShaderFunction.h"
#include "ShaderFunction.h"
#include "ShaderStringFactory.h"
#include "InputNode.h"
#include "OutputNode.h"

namespace Pixel {
	LerpShaderFunction::LerpShaderFunction()
	{
		m_InputNodeDisplayColor = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)};
		m_InputNodeDisplayName = { "X", "Y", "S"};

		m_OutputNodeDisplayColor = { glm::vec3(1.0f, 1.0f, 1.0f) };
		m_OutputNodeDisplayName = { "" };

		m_HeaderColor = glm::vec3(0.75f, 0.82f, 0.93f);
	}

	LerpShaderFunction::LerpShaderFunction(const std::string& showName, Ref<Material> pMaterial)
	: ShaderFunction(showName, pMaterial)
	{
		m_InputNodeDisplayColor = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
		m_InputNodeDisplayName = { "X", "Y", "S" };

		m_OutputNodeDisplayColor = { glm::vec3(1.0f, 1.0f, 1.0f) };
		m_OutputNodeDisplayName = { "" };

		m_HeaderColor = glm::vec3(0.75f, 0.82f, 0.93f);
	}

	LerpShaderFunction::~LerpShaderFunction()
	{

	}

	bool LerpShaderFunction::GetFunctionString(std::string& OutString) const
	{
		OutString += m_pOutputs[0]->GetNodeName() + " = lerp(" + m_pInputs[0]->GetNodeName() + ", " + m_pInputs[1]->GetNodeName() + ", " + m_pInputs[2]->GetNodeName() + ");\n";

		return true;
	}

	bool LerpShaderFunction::ResetValueType() const
	{
		//to expand
		uint32_t maxType = (uint32_t)ValueType::VT_1;

		for (uint32_t i = 0; i < m_pInputs.size(); ++i)
		{
			if (m_pInputs[i]->GetOutputLink())
			{
				if (maxType < (uint32_t)m_pInputs[i]->GetOutputLink()->GetValueType())
				{
					maxType = (uint32_t)m_pInputs[i]->GetOutputLink()->GetValueType();
				}
			}
		}

		for (uint32_t i = 0; i < m_pInputs.size(); ++i)
		{
			m_pInputs[i]->SetValueType(static_cast<ValueType>(maxType));
		}

		m_pOutputs[0]->SetValueType(static_cast<ValueType>(maxType));

		return true;
	}

	void LerpShaderFunction::ConstructPutNodeAndSetPutNodeOwner()
	{
		ShaderFunction::ConstructPutNodeAndSetPutNodeOwner();

		//default is vt_4
		std::string InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string InputName = "X" + InputId;
		Ref<InputNode> pInputNode = nullptr;
		pInputNode = CreateRef<InputNode>(ValueType::VT_4, InputName, shared_from_this());
		m_pInputs.push_back(pInputNode);
		//m_InputNodeDisplayName.push_back("A");
		//m_InputNodeDisplayColor.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

		InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		InputName = "Y" + InputId;
		pInputNode = nullptr;
		pInputNode = CreateRef<InputNode>(ValueType::VT_4, InputName, shared_from_this());
		m_pInputs.push_back(pInputNode);
		//m_InputNodeDisplayName.push_back("B");
		//m_InputNodeDisplayColor.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

		InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		InputName = "S" + InputId;
		pInputNode = nullptr;
		pInputNode = CreateRef<InputNode>(ValueType::VT_4, InputName, shared_from_this());
		m_pInputs.push_back(pInputNode);

		std::string OutputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "LerpOutput" + OutputId;
		Ref<OutputNode> pOutputNode = nullptr;
		pOutputNode = CreateRef<OutputNode>(ValueType::VT_4, OutputName, shared_from_this());
		m_pOutputs.push_back(pOutputNode);
		//m_OutputNodeDisplayName.push_back("");
		//m_OutputNodeDisplayColor.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	}
}