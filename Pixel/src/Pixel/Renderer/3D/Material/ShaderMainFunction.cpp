#include "pxpch.h"

#include "ShaderMainFunction.h"

namespace Pixel
{
	bool ShaderMainFunction::GetInputValueString(std::string& OutString, uint32_t uiOutPutStringType)
	{
		std::string Temp;
		if (uiOutPutStringType == OST_MATERIAL)
		{
			for (uint32_t i = 0; i < m_pInput.size(); ++i)
			{
				if (m_pInput[i] == GetNormalNode())
					continue;
				if (m_pInput[i]->GetValueType() == PutNode::VT_1)
				{
					OutString += Renderer3D::Float() + " ";

					Temp = Renderer3D::FloatConst("0");
				}
				else if (m_pInput[i]->GetValueType() == PutNode::VT_2)
				{
					OutString += Renderer3D::Float2() + " ";

					Temp = Renderer3D::FloatConst2("0", "0");
				}
				else if (m_pInput[i]->GetValueType() == PutNode::VT_3)
				{
					OutString += Renderer3D::Float3() + " ";

					Temp = Renderer3D::FloatConst3("0", "0", "0");
				}
				else if (m_pInput[i]->GetValueType() == PutNode::VT_4)
				{
					OutString += Renderer3D::Float4() + " ";

					Temp = Renderer3D::FloatConst4("0", "0", "0", "0");
				}

				if (!m_pInput[i]->GetOutputLink())
				{
					OutString += m_pInput[i]->GetNodeName() + " = " + Temp + ";\n";
					continue;
				}

				OutString += GetValueEqualString(m_pInput[i]->GetOutputLink(), m_pInput[i]);
			}
		}

		return true;
	}

	//------Declare nonuser custom variable------
	//------float3 WorldPos------
	//------float3 WorldNormal------
	void ShaderMainFunction::GetValueUseDeclareString(std::string& OutString, uint32_t uiValueUseString)
	{
		std::string DefaultValue = Renderer3D::FloatConst3("0", "0", "0");
		if ((uiValueUseString & VUS_WORLD_POS) == VUS_WORLD_POS)
		{
			OutString += Renderer3D::Float3() + ShaderStringFactory::m_WorldPos + " = " + DefaultValue + ";\n";
		}

		if ((uiValueUseString & VUS_WORLD_NORMAL) == VUS_WORLD_NORMAL)
		{
			OutString += Renderer3D::Float3() + ShaderStringFactory::m_WorldNormal + " = " + DefaultValue + ";\n";
		}
	}
	//------Declare nonuser custom variable------

	bool ShaderMainFunction::GetShaderTreeString(std::string& OutString, uint32_t uiOutPutStringType)
	{
		if (m_bIsVisited == true)
			return true;
		else
		{
			m_bIsVisited = true;
			if (uiOutPutStringType == OST_MATERIAL)
			{
				//declare nonuser custom variable
				//WorldPos¡¢WorldNormal etc.
				GetValueUseDeclareString(OutString, VUS_ALL);

				//get main function input node's input code
				//becase WorldNormal need this, and roughness and metallic are also need this
				GetNormalString(OutString);

				GetValueUseString(OutString, VUS_ALL);

				for (uint32_t i = 0; i < m_pInput.size(); ++i)
				{
					if (m_pInput[i]->GetOutputLink() == nullptr)
						continue;
					else if (m_pInput[i] == GetNormalNode())
					{
						continue;
					}
					else
					{
						(std::static_pointer_cast<ShaderFunction>(m_pInput[i]->GetOutputLink()->GetOwner()))->GetShaderTreeString(OutString);
					}
				}
			}
		}

		//input temp variable declare
		if (!GetInputValueString(OutString, uiOutPutStringType))
			return false;

		//output temp variable declare
		if (!GetOutputValueString(OutString))
			return false;

		//calculate output temp variable from input temp variable
		if (uiOutPutStringType == OST_MATERIAL)
		{
			if (!GetFunctionString(OutString))
				return false;
		}
		return true;
	}

	void ShaderMainFunction::GetValueUseString(std::string& OutString, uint32_t uiOutPutStringType)
	{
		if ((uiOutPutStringType & VUS_WORLD_NORMAL) == VUS_WORLD_NORMAL)
		{
			if (ShaderStringFactory::m_PSInputLocalNormal.size() == 0)
			{
				OutString += ShaderStringFactory::m_WorldNormal + " = " +
					Renderer3D::FloatConst3("0", "0", "0") + ";\n";
			}
		}

		if ((uiOutPutStringType & VUS_WORLD_POS) == VUS_WORLD_POS)
		{
			//OutString += "	" + ShaderStringFactory::m_WorldPos + " = ";
			//Local World To Pos
			//ShaderStringFactory::LocalToWorldPos(ShaderStringFactory::m_PSInputLocalNormal, OutString);
			//OutString += ";\n";
		}
	}

	void ShaderMainFunction::GetNormalString(std::string& OutString)
	{
		if (GetNormalNode()->GetOutputLink())
		{
			(std::static_pointer_cast<ShaderFunction>(GetNormalNode()->GetOutputLink()->GetOwner()))->GetShaderTreeString(OutString);
		}
	}
}