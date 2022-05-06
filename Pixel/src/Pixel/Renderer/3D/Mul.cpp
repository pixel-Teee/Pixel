#include "pxpch.h"
#include "Mul.h"

#include "Pixel/Renderer/3D/ShaderStringFactory.h"

namespace Pixel {

	Mul::Mul()
	{
		m_functionType = ShaderFunction::Mul;
	}

	Mul::Mul(const std::string& showName, Ref<Material> pMaterial):ShaderFunction(showName, pMaterial)
	{
		m_functionType = ShaderFunction::Mul;
	}

	Mul::~Mul()
	{

	}

	bool Mul::GetFunctionString(std::string& OutString) const
	{
		OutString += m_pOutput[0]->GetNodeName() + " = " +
			m_pInput[0]->GetNodeName() + " * " + m_pInput[1]->GetNodeName() + ";\n";
		return true;
	}

	bool Mul::ResetValueType() const
	{
		uint32_t MaxType = PutNode::VT_1;
		for (uint32_t i = 0; i < m_pInput.size(); ++i)
		{
			if (m_pInput[i]->GetOutputLink())
			{
				if (MaxType < m_pInput[i]->GetOutputLink()->GetValueType())
				{
					MaxType = m_pInput[i]->GetOutputLink()->GetValueType();
				}
			}
		}
		for (uint32_t i = 0; i < m_pInput.size(); ++i)
		{
			m_pInput[i]->SetValueType((PutNode::ValueType)MaxType);
		}
		m_pOutput[0]->SetValueType((PutNode::ValueType)MaxType);
		return true;
	}

	void Mul::ConstrcutPutNodeAndSetPutNodeOwner()
	{
		ShaderFunction::ConstrcutPutNodeAndSetPutNodeOwner();
		
		//default is vt_4
		std::string InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string InputName = "MulInputA" + InputId;
		Ref<InputNode> pInputNode = nullptr;
		pInputNode = CreateRef<InputNode>(PutNode::VT_4, InputName, shared_from_this());
		m_pInput.push_back(pInputNode);

		++ShaderStringFactory::m_ShaderValueIndex;
		InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		InputName = "MulInputB" + InputId;
		pInputNode = nullptr;
		pInputNode = CreateRef<InputNode>(PutNode::VT_4, InputName, shared_from_this());
		m_pInput.push_back(pInputNode);

		++ShaderStringFactory::m_ShaderValueIndex;
		std::string OutputId = std::to_string(ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "MulOutput" + OutputId;
		Ref<OutputNode> pOutputNode = nullptr;
		pOutputNode = CreateRef<OutputNode>(PutNode::VT_4, OutputName, shared_from_this());
		m_pOutput.push_back(pOutputNode);
	}

}