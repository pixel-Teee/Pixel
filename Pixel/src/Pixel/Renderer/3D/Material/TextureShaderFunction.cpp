#include "pxpch.h"

#include "TextureShaderFunction.h"
#include "InputNode.h"
#include "OutputNode.h"
#include "ShaderStringFactory.h"

namespace Pixel {
	TextureShaderFunction::TextureShaderFunction()
	{

	}

	TextureShaderFunction::TextureShaderFunction(const std::string& ShowName, Ref<Material> pMaterial)
		: ShaderFunction(ShowName, pMaterial)
	{
	}

	TextureShaderFunction::~TextureShaderFunction()
	{

	}

	bool TextureShaderFunction::GetInputValueString(std::string& OutString) const
	{
		//uv coordinate
		std::string temp;
		if (m_pInputs[0]->GetValueType() == ValueType::VT_1)
		{
			OutString += ShaderStringFactory::Float() + " ";
			temp = ShaderStringFactory::FloatConst("0");
		}
		else if (m_pInputs[0]->GetValueType() == ValueType::VT_2)
		{
			OutString += ShaderStringFactory::Float2() + " ";
			temp = ShaderStringFactory::FloatConst2("0", "0");
		}
		else if (m_pInputs[0]->GetValueType() == ValueType::VT_3)
		{
			OutString += ShaderStringFactory::Float3() + " ";
			temp = ShaderStringFactory::FloatConst3("0", "0", "0");
		}
		else if (m_pInputs[0]->GetValueType() == ValueType::VT_4)
		{
			OutString += ShaderStringFactory::Float4() + " ";
			temp = ShaderStringFactory::FloatConst4("0", "0", "0", "0");
		}
		else
			return false;

		//don't have link, then use default uv coordinate
		if (!m_pInputs[0]->GetOutputLink())
		{
			//TODO:in the future, fix this hard code pin.TexCoord;
			//OutString += m_pInputs[0]->GetNodeName()
			OutString += m_pInputs[0]->GetNodeName() + " = " + "pin.TexCoord;\n";
		}

		//otherwise, use link
		OutString += GetValueEqualString(m_pInputs[0]->GetOutputLink(), m_pInputs[0]);

		return true;
	}

	bool TextureShaderFunction::GetDeclareString(std::string& OutString, int32_t registerId) const
	{
		if (registerId != -1)
		{
			OutString += "Texture2D " + m_ShowName + " : register(t" + std::to_string(registerId) + ");\n";
		}
		else
		{
			OutString += "Texture2D " + m_ShowName + ";\n";
		}
		return true;
	}

}