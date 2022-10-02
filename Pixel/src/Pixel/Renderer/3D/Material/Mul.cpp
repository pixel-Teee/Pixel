#include "pxpch.h"

//------my library------
#include "Mul.h"
#include "ShaderFunction.h"
#include "ShaderStringFactory.h"
#include "InputNode.h"
#include "OutputNode.h"
//------my library------

namespace Pixel {

	Mul::Mul()
	{
		//m_functionType = ShaderFunctionType::Mul;
		m_InputNodeDisplayColor = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)};
		m_InputNodeDisplayName = { "A", "B" };

		m_OutputNodeDisplayColor = { glm::vec3(1.0f, 1.0f, 1.0f) };
		m_OutputNodeDisplayName = { "" };

		m_HeaderColor = glm::vec3(0.75f, 0.82f, 0.93f);
	}

	Mul::Mul(const std::string& showName, Ref<Material> pMaterial) : ShaderFunction(showName, pMaterial)
	{
		//m_functionType = ShaderFunctionType::Mul;
		m_InputNodeDisplayColor = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
		m_InputNodeDisplayName = { "A", "B" };

		m_OutputNodeDisplayColor = { glm::vec3(1.0f, 1.0f, 1.0f) };
		m_OutputNodeDisplayName = { "" };

		m_HeaderColor = glm::vec3(0.75f, 0.82f, 0.93f);
	}

	Mul::~Mul()
	{

	}

	bool Mul::GetFunctionString(std::string& OutString) const
	{
		PX_CORE_ASSERT(m_pOutputs.size() >= 1, "out of the range!");
		PX_CORE_ASSERT(m_pInputs.size() >= 2, "out of the range!");

		OutString += m_pOutputs[0]->GetNodeName() + " = " + m_pInputs[0]->GetNodeName() + " * " + m_pInputs[1]->GetNodeName() + ";\n";
		//OutputNodeName = InputNodeName0 + InputNodeName1;
		return true;
	}

	bool Mul::ResetValueType() const
	{
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

		return true;
	}

	void Mul::ConstructPutNodeAndSetPutNodeOwner()
	{
		ShaderFunction::ConstructPutNodeAndSetPutNodeOwner();
		
		//default is vt_4
		std::string InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string InputName = "MulInputA" + InputId;
		Ref<InputNode> pInputNode = nullptr;
		pInputNode = CreateRef<InputNode>(ValueType::VT_4, InputName, shared_from_this());
		m_pInputs.push_back(pInputNode);
		//m_InputNodeDisplayName.push_back("A");
		//m_InputNodeDisplayColor.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

		InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		InputName = "MulInputB" + InputId;
		pInputNode = nullptr;
		pInputNode = CreateRef<InputNode>(ValueType::VT_4, InputName, shared_from_this());
		m_pInputs.push_back(pInputNode);
		//m_InputNodeDisplayName.push_back("B");
		//m_InputNodeDisplayColor.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

		std::string OutputId = std::to_string(ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "MulOutput" + OutputId;
		Ref<OutputNode> pOutputNode = nullptr;
		pOutputNode = CreateRef<OutputNode>(ValueType::VT_4, OutputName, shared_from_this());
		m_pOutputs.push_back(pOutputNode);
		//m_OutputNodeDisplayName.push_back("");
		//m_OutputNodeDisplayColor.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	}

}