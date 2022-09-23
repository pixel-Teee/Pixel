#include "pxpch.h"
#include "ShaderStringFactory.h"

//------cpp library------
#include <iostream>
#include <fstream>
#include <string>
//------cpp library------

#include "Material.h"
#include "PutNode.h"

namespace Pixel {
	uint32_t ShaderStringFactory::m_ShaderValueIndex;

	void ShaderStringFactory::Init()
	{
		m_ShaderValueIndex = 0;
	}

	std::string ShaderStringFactory::Float()
	{
		return std::string("float ");
	}

	std::string ShaderStringFactory::Float2()
	{
		return std::string("float2 ");
	}

	std::string ShaderStringFactory::Float3()
	{
		return std::string("float3 ");
	}

	std::string ShaderStringFactory::Float4()
	{
		return std::string("float4 ");
	}

	std::string ShaderStringFactory::Return()
	{
		return std::string("return ");
	}

	std::string ShaderStringFactory::FloatConst(const std::string& value)
	{
		return value;
	}

	std::string ShaderStringFactory::FloatConst2(const std::string& value1, const std::string& value2)
	{
		return std::string("float2") + "(" + value1 + ", " + value2 + ")";//float2(value1, value2)
	}

	std::string ShaderStringFactory::FloatConst3(const std::string& value1, const std::string& value2,
		const std::string& value3)
	{
		return std::string("float3") + "(" + value1 + ", " + value2 + ", " + value3 + ")";//float3(value1, value2, value3)
	}

	std::string ShaderStringFactory::FloatConst4(const std::string& value1, const std::string& value2,
		const std::string& value3, const std::string& value4)
	{
		return std::string("float4") + "(" + value1 + ", " + value2 + ", " + value3 + ", " + value4 + ")";//float4(value1, value2, value3, value4)
	}

	std::string ShaderStringFactory::GetValueElement(Ref<PutNode> pPutNode, ValueElement valueElement)
	{
		//extract the pPutNode's value element
		std::string Temp = pPutNode->GetNodeName();

		if(valueElement > ValueElement::VE_NONE)
		{
			if(pPutNode->GetValueType() == ValueType::VT_1 && (valueElement & ValueElement::VE_R))
			{
				return Temp;
			}
		}

		std::string Value[4];
		Value[0] = "x";
		Value[1] = "y";
		Value[2] = "z";
		Value[3] = "w";

		ValueElement Mask[4];
		Mask[0] = VE_R;
		Mask[1] = VE_G;
		Mask[2] = VE_B;
		Mask[3] = VE_A;
		Temp += (".");

		//value element: need to extracted element
		for (uint32_t i = 0; i < 4; ++i)
		{
			if (i <= (uint32_t)pPutNode->GetValueType())
			{
				if (valueElement & Mask[i])
				{
					Temp += Value[i];
				}
			}
		}

		return Temp;
	}
	std::string ShaderStringFactory::CreateDeferredGeometryShaderString(Ref<Material> pMaterial)
	{
		//create deferred geometry shader
		std::ifstream geometryPassIncludes("assets/shaders/Common/GeomertyPass1.hlsl");
		std::stringstream buffer;
		buffer << geometryPassIncludes.rdbuf();
		std::string out = buffer.str();
		geometryPassIncludes.close();
		pMaterial->GetShaderTreeString(out);
		out += "return pixelOut;\n}\n";

		//write to cache
		std::ofstream cache("assets/shaders/Cache/test.hlsl");
		cache << out;
		cache.close();

		//TODO:in the future, will in there to compile shader
		//Ref<Shader> testCompilerVertex = Shader::Create("assets/shaders/Cache/test.hlsl", "VS", "vs_5_0");
		//Ref<Shader> testCompilerFrag = Shader::Create("assets/shaders/Cache/test.hlsl", "PS", "ps_5_0");

		return out;
	}
}
