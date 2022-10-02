#include "pxpch.h"
#include "ShaderStringFactory.h"

//------cpp library------
#include <iostream>
#include <fstream>
#include <string>
//------cpp library------

#include "Material.h"
#include "PutNode.h"
#include "Pixel/Core/Application.h"
#include "Pixel/Renderer/BaseRenderer.h"
#include "Pixel/Renderer/3D/StaticMesh.h"
#include "Texture2DShaderFunction.h"
#include "OutputNode.h"
#include "InputNode.h"

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

	std::string ShaderStringFactory::Tex2D(Texture2DShaderFunction* pTexture2DShaderFunction)
	{
		return pTexture2DShaderFunction->GetOutputNode(Texture2DShaderFunction::OUT_COLOR)->GetNodeName()
			+ " = " + pTexture2DShaderFunction->GetShowName() + ".Sample(gsamPointWrap, " + pTexture2DShaderFunction->GetInputNode(Texture2DShaderFunction::IN_TEXCOORD)->GetNodeName() + ");\n";
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
	std::string ShaderStringFactory::CreateDeferredGeometryShaderString(Ref<Material> pMaterial, Ref<StaticMesh> pStaticMesh)
	{
		//create deferred geometry shader
		std::ifstream geometryPassIncludes("assets/shaders/Common/GeomertyPass1.hlsl");
		std::stringstream buffer;
		buffer << geometryPassIncludes.rdbuf();
		std::string out = buffer.str();

		geometryPassIncludes.close();

		std::string PixelShaderInclude;//just for include, some useful shader
		std::string PixelShaderDynamic;//TODO:not used temporarily
		std::string PixelShaderInputDeclare;//pixel shader input declare
		std::string PixelShaderOutputDeclare;//pixel shader output declare, this will be fixed
		std::string PixelShaderConstantString;//pxiel shader constant string
		std::string PixelShaderFunctionString;//pixel shader function string

		//PixelShaderInclude:from shader string factory to get common shader path
		GetIncludeShader(PixelShaderInclude);
		//PixelShaderInputDeclare:interms of the static mesh's vertex buffer's layout to create pixel shader input declare(x)

		//PixelShaderOutputDeclare:this is fixed
		pMaterial->GetShaderTreeString(PixelShaderFunctionString);
		//PixelShaderFunctionString:get shader tree string, then get the cbuffer
		CreatePixelShaderUserConstant(PixelShaderConstantString, pMaterial);
		//PixelShaderConstantString:cbuffer declare
				
		out += PixelShaderInclude + "\n";
		out += PixelShaderConstantString + "\n";
		out += "PixelOut PS(VertexOut pin){\n";
		//pMaterial->GetShaderTreeString(out);
		out += PixelShaderFunctionString;
		out += "return pixelOut;\n}";

		//write to cache
		std::ofstream cache("assets/shaders/Cache/test.hlsl");
		cache << out;
		cache.close();

		//TODO:in the future, will in there to compile shader
		Ref<Shader> testCompilerVertex = Shader::Create("assets/shaders/Cache/test.hlsl", "VS", "vs_5_0");
		Ref<Shader> testCompilerFrag = Shader::Create("assets/shaders/Cache/test.hlsl", "PS", "ps_5_0");

		pMaterial->m_PsoIndex = Application::Get().GetRenderer()->CreateMaterialPso(testCompilerVertex, testCompilerFrag, pMaterial->m_PsoIndex);
		pMaterial->dirty = true;

		return out;
	}
	void ShaderStringFactory::GetIncludeShader(std::string& Out)
	{
		Out = "#include \"../Common/Common.hlsl\"";
	}
	void ShaderStringFactory::CreatePixelShaderUserConstant(std::string& Out, Ref<Material> pMaterial)
	{
		std::string temp = "cbuffer CbMaterial : register(b2)\n{\n";

		pMaterial->CreateConstValueDeclare(temp);

		//other information
		temp += "bool HaveNormal;\n";

		temp += "int ShadingModelID;\n";

		//TODO:in the future, in terms shading model to add these shader parameter
		temp += "float ClearCoat;\n";
		temp += "float ClearCoatRoughness;\n";

		temp += "};\n";

		Out += temp;

		temp.clear();//clear string

		uint32_t registerId = 0;
		
		pMaterial->CreateTextureDeclare(temp, registerId);

		Out += temp;
	}
}
