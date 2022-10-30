#include "pxpch.h"

#include "ComponentMask.h"
#include "ShaderFunction.h"
#include "ShaderStringFactory.h"
#include "InputNode.h"
#include "OutputNode.h"

namespace Pixel {
	ComponentMask::ComponentMask()
	{
		m_InputNodeDisplayColor = { glm::vec3(1.0f, 0.0f, 0.0f) };
		m_InputNodeDisplayName = { "value"};//value

		m_OutputNodeDisplayColor = { glm::vec3(1.0f, 1.0f, 1.0f) };
		m_OutputNodeDisplayName = { "" };

		m_HeaderColor = glm::vec3(0.75f, 0.82f, 0.93f);
	}

	ComponentMask::ComponentMask(const std::string& showName, Ref<Material> pMaterial) : ShaderFunction(showName, pMaterial)
	{
		m_InputNodeDisplayColor = { glm::vec3(1.0f, 0.0f, 0.0f) };
		m_InputNodeDisplayName = { "value" };//value

		m_OutputNodeDisplayColor = { glm::vec3(1.0f, 1.0f, 1.0f) };
		m_OutputNodeDisplayName = { "" };

		m_HeaderColor = glm::vec3(0.75f, 0.82f, 0.93f);
	}

	ComponentMask::~ComponentMask()
	{

	}

	bool ComponentMask::GetFunctionString(std::string& OutString) const
	{
		std::string temp =
			(R ? std::string("r") : std::string("")) +
			(G ? std::string("g") : std::string("")) +
			(B ? std::string("b") : std::string("")) +
			(A ? std::string("a") : std::string(""));

		//swizzle
		OutString += m_pOutputs[0]->GetNodeName() + " = " + m_pInputs[0]->GetNodeName() + "." + temp + ";\n";
		return true;
	}

	void ComponentMask::ConstructPutNodeAndSetPutNodeOwner()
	{
		ShaderFunction::ConstructPutNodeAndSetPutNodeOwner();

		//default is vt_4
		std::string InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string InputName = "InputValue" + InputId;
		Ref<InputNode> pInputNode = nullptr;
		pInputNode = CreateRef<InputNode>(ValueType::VT_4, InputName, shared_from_this());
		m_pInputs.push_back(pInputNode);

		std::string OutputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "OutputValue" + OutputId;
		Ref<OutputNode> pOutputNode = nullptr;
		pOutputNode = CreateRef<OutputNode>(ValueType::VT_4, OutputName, shared_from_this());
		m_pOutputs.push_back(pOutputNode);
	}

	void ComponentMask::SetMask(uint32_t maskIndex, bool enable)
	{
		if (maskIndex == 0) R = enable;
		else if (maskIndex == 1) G = enable;
		else if (maskIndex == 2) B = enable;
		else if (maskIndex == 3) A = enable;

		//reconstruct output node value type
		PX_CORE_ASSERT(m_pOutputs.size() == 1, "output node number error!");

		int32_t valueType = (R ? 1 : 0) + (G ? 1 : 0) + (B ? 1 : 0) + (A ? 1 : 0);
		if (valueType == 1)
			m_pOutputs[0]->SetValueType(ValueType::VT_1);
		else if (valueType == 2)
			m_pOutputs[0]->SetValueType(ValueType::VT_2);
		else if (valueType == 3)
			m_pOutputs[0]->SetValueType(ValueType::VT_3);
		else
			m_pOutputs[0]->SetValueType(ValueType::VT_4);
	}

	void ComponentMask::UpdateOutputNodeValueType()
	{
		PX_CORE_ASSERT(m_pOutputs.size() == 1, "output node number error!");

		int32_t valueType = (R ? 1 : 0) + (G ? 1 : 0) + (B ? 1 : 0) + (A ? 1 : 0);
		if (valueType == 1)
			m_pOutputs[0]->SetValueType(ValueType::VT_1);
		else if (valueType == 2)
			m_pOutputs[0]->SetValueType(ValueType::VT_2);
		else if (valueType == 3)
			m_pOutputs[0]->SetValueType(ValueType::VT_3);
		else
			m_pOutputs[0]->SetValueType(ValueType::VT_4);
	}
}