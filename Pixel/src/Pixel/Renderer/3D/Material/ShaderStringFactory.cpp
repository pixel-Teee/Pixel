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
#include "ShaderMainFunction.h"

namespace Pixel {
	uint32_t ShaderStringFactory::m_ShaderValueIndex;

	bool ShaderStringFactory::m_GenerateIntermediateNodesResult;

	void ShaderStringFactory::Init()
	{
		m_ShaderValueIndex = 0;
		m_GenerateIntermediateNodesResult = false;
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

	std::string ShaderStringFactory::Tex2D(Texture2DShaderFunction* pTexture2DShaderFunction, bool decodeNormal)
	{
		std::string result;//return result

		if (decodeNormal)
		{
			result += "#if HAVE_NORMAL > 0\n";
			result += pTexture2DShaderFunction->GetOutputNode(Texture2DShaderFunction::OUT_COLOR)->GetNodeName()
				+ " = " + "DecodeNormalMap(pin.NormalW, pin.TangentW, " + pTexture2DShaderFunction->GetShowName() + ".Sample(gsamPointWrap, " + pTexture2DShaderFunction->GetInputNode(Texture2DShaderFunction::IN_TEXCOORD)->GetNodeName() + ") * 2.0f - 1.0f)" + ";\n";
			result += "#else\n";
			result += pTexture2DShaderFunction->GetOutputNode(Texture2DShaderFunction::OUT_COLOR)->GetNodeName()
				+ " = " + pTexture2DShaderFunction->GetShowName() + ".Sample(gsamPointWrap, " + pTexture2DShaderFunction->GetInputNode(Texture2DShaderFunction::IN_TEXCOORD)->GetNodeName() + ");\n";
			result += "#endif\n";
		}
		else
		{
			result += pTexture2DShaderFunction->GetOutputNode(Texture2DShaderFunction::OUT_COLOR)->GetNodeName()
				+ " = " + pTexture2DShaderFunction->GetShowName() + ".Sample(gsamPointWrap, " + pTexture2DShaderFunction->GetInputNode(Texture2DShaderFunction::IN_TEXCOORD)->GetNodeName() + ");\n";
		}
		return result;
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
				
		out += "\n" + PixelShaderInclude + "\n";
		out += PixelShaderConstantString + "\n";
		out += "PixelOut PS(VertexOut pin){\n";
		//pMaterial->GetShaderTreeString(out);
		out += PixelShaderFunctionString;
		out += "return pixelOut;\n}";

		std::string shaderPath = "assets/shaders/Cache/" + pMaterial->m_MaterialName + ".hlsl";

		//write to cache
		std::ofstream cache(shaderPath);
		cache << out;
		cache.close();

		//TODO:in the future, will in there to compile shader
		Ref<Shader> testCompilerVertex = Shader::Create(shaderPath, "VS", "vs_5_0");
		Ref<Shader> testCompilerFrag = Shader::Create(shaderPath, "PS", "ps_5_0");

		//TODO:pass preview model's buffer layout
		//Application::Get().GetRenderer()->CreateMaterialPso(testCompilerVertex, testCompilerFrag, pMaterial, );
		//pMaterial->m_pVertexShader = testCompilerVertex;
		//pMaterial->m_pPixelShader = testCompilerFrag;
		//pMaterial->dirty = true;
		pMaterial->LinkAllParameters();

		return out;
	}
	std::string ShaderStringFactory::CreateDeferredGeometryVertexShaderString(Ref<Material> pMaterial)
	{
		//create deferred geometry shader
		std::ifstream geometryPassIncludes("assets/shaders/Common/GeomertyPassVertex.hlsl");
		std::stringstream buffer;
		buffer << geometryPassIncludes.rdbuf();
		std::string out;

		std::string VertexShaderInclude;

		GetIncludeShader(VertexShaderInclude);

		out += VertexShaderInclude + "\n";
		out += buffer.str();

		geometryPassIncludes.close();

		//TODO:need to fix in there
		//pMaterial->m_PsoIndex = Application::Get().GetRenderer()->CreateMaterialPso(testCompilerVertex, testCompilerFrag, pMaterial->m_PsoIndex);
		//pMaterial->m_pVertexShader = testCompilerVertex;
		//pMaterial->m_pPixelShader = testCompilerFrag;
		//pMaterial->dirty = true;
		//pMaterial->LinkAllParameters();	

		return out;
	}
	std::string ShaderStringFactory::CreateDeferredGeometryPixelShaderString(Ref<Material> pMaterial)
	{
		//create deferred geometry shader
		std::ifstream geometryPassIncludes("assets/shaders/Common/GeomertyPassPixel.hlsl");
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
		out += "SamplerState gsamPointWrap : register(s0);\n";
		out += "PixelOut PS(VertexOut pin){\n";
		//pMaterial->GetShaderTreeString(out);
		out += PixelShaderFunctionString;
		out += "return pixelOut;\n}";

		//std::string shaderPath = "assets/shaders/Cache/" + pMaterial->m_MaterialName + ".hlsl";

		//write to cache
		//std::ofstream cache(shaderPath);
		//cache << out;
		//cache.close();

		//TODO:need to fix these
		//TODO:in the future, will in there to compile shader
		//Ref<Shader> testCompilerVertex = Shader::Create(shaderPath, "VS", "vs_5_0");
		//Ref<Shader> testCompilerFrag = Shader::Create(shaderPath, "PS", "ps_5_0");

		//pMaterial->m_PsoIndex = Application::Get().GetRenderer()->CreateMaterialPso(testCompilerVertex, testCompilerFrag, pMaterial->m_PsoIndex);
		//pMaterial->m_pVertexShader = testCompilerVertex;
		//pMaterial->m_pPixelShader = testCompilerFrag;
		//pMaterial->dirty = true;
		//pMaterial->LinkAllParameters();

		return out;
	}

	void ShaderStringFactory::GenerateIntermediateShaderString(Ref<Material> pMaterial, Ref<StaticMesh> pStaticMesh)
	{
		//create deferred geometry shader
		std::ifstream geometryPassIncludes("assets/shaders/Common/GeometryPassQuad.hlsl");
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
		//GetIncludeShader(PixelShaderInclude);
		//PixelShaderInputDeclare:interms of the static mesh's vertex buffer's layout to create pixel shader input declare(x)

		//PixelShaderOutputDeclare:this is fixed
		pMaterial->GetShaderTreeString(PixelShaderFunctionString);
		//PixelShaderFunctionString:get shader tree string, then get the cbuffer
		CreatePixelShaderUserConstant(PixelShaderConstantString, pMaterial);
		//PixelShaderConstantString:cbuffer declare

		//out += "\n#include \"../../Common/Common.hlsl\"\n";
		//out += "\n" + PixelShaderInclude + "\n";
		out += "\n" + PixelShaderConstantString;
		//out += "SamplerState gsamPointWrap : register(s0);\n";
		out += "PixelOut PS(VertexOut pin){\n";
		//pMaterial->GetShaderTreeString(out);
		//out += PixelShaderFunctionString;
		//out += "return pixelOut;\n}";
		out += "PixelOut pixelOut = (PixelOut)(0.0f);\n";

		for (uint32_t i = 0; i < pMaterial->GetShaderFunction().size(); ++i)
		{
			pMaterial->GetShaderFunction()[i]->m_IntermediateShaderString.clear();
			std::string result = out;//result
			//pMaterial->GetMainFunction()->ClearShaderTreeStringFlag();
			for (uint32_t j = 0; j < pMaterial->GetShaderFunction().size(); ++j)
				pMaterial->GetShaderFunction()[j]->ClearVisit();
			//skip shader main function
			if (pMaterial->GetShaderFunction()[i]->GetOutputNodeNum() == 0) continue;
			std::string temp;//temp

			//temp = pMaterial->GetShaderFunction()[i]->GetOutputNode(0)->GetNodeName() + ";\n";
			//copy to intermediate shader string
			//will call SetValueType
			pMaterial->GetShaderFunction()[i]->GetShaderTreeString(pMaterial->GetShaderFunction()[i]->m_IntermediateShaderString);

			if (pMaterial->GetShaderFunction()[i]->GetOutputNode(0)->GetValueType() == ValueType::VT_1)
			{
				temp = "float4(" + pMaterial->GetShaderFunction()[i]->GetOutputNode(0)->GetNodeName() + ", 0.0f, 0.0f, 1.0f);\n";
			}
			else if (pMaterial->GetShaderFunction()[i]->GetOutputNode(0)->GetValueType() == ValueType::VT_2)
			{
				temp = "float4(" + pMaterial->GetShaderFunction()[i]->GetOutputNode(0)->GetNodeName() + ", 0.0f, 1.0f);\n";
			}
			else if (pMaterial->GetShaderFunction()[i]->GetOutputNode(0)->GetValueType() == ValueType::VT_3)
			{
				temp = "float4(" + pMaterial->GetShaderFunction()[i]->GetOutputNode(0)->GetNodeName() + ", 1.0f);\n";
			}
			else if (pMaterial->GetShaderFunction()[i]->GetOutputNode(0)->GetValueType() == ValueType::VT_4)
			{
				temp = pMaterial->GetShaderFunction()[i]->GetOutputNode(0)->GetNodeName() + ";\n";
			}

			pMaterial->GetShaderFunction()[i]->m_IntermediateShaderString += "pixelOut.finalColor = " + temp;//copy to 
			pMaterial->GetShaderFunction()[i]->m_IntermediateShaderString += "return pixelOut;\n}";
			pMaterial->GetShaderFunction()[i]->m_IntermediateShaderString = result + pMaterial->GetShaderFunction()[i]->m_IntermediateShaderString;
			//PIXEL_CORE_INFO("{0}******", pMaterial->GetShaderFunction()[i]->m_IntermediateShaderString);
		}
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
