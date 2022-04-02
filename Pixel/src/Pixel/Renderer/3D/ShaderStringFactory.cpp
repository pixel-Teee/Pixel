#include "pxpch.h"
#include "ShaderStringFactory.h"
#include "RendererPass.h"

#include <iostream>
#include <fstream>
#include <string>

namespace Pixel {
	uint32_t ShaderStringFactory::m_ShaderValueIndex;
	uint32_t ShaderStringFactory::m_CreateShaderNum;
	std::string ShaderStringFactory::m_Model;
	std::string ShaderStringFactory::m_ViewProjection;
	std::string ShaderStringFactory::m_WorldPos;
	std::string ShaderStringFactory::m_WorldNormal;
	std::string ShaderStringFactory::m_PSInputLocalNormal;
	std::string ShaderStringFactory::m_WorldMatrix;
	std::string ShaderStringFactory::m_PSOutputColorValue;
	void ShaderStringFactory::Init()
	{
		m_Model = "u_Model";
		m_ViewProjection = "u_ViewProjection";
		m_CreateShaderNum = 0;
		m_WorldPos = "WorldPos";
		m_WorldNormal = "WorldNormal";
	}

	//------Include Shader------
	void ShaderStringFactory::GetIncludeShader(std::string& OutString)
	{
		OutString += "#type vertex\n";
		OutString += "#version 450 core\n";
	}

	//------Input Declare------

	void ShaderStringFactory::CreateVOutputDeclare(MaterialShaderPara& MSPara, uint32_t uiPassType, std::string& OutString)
	{
		std::string TempDeclare;
		//world position
		OutString += "layout(location = 0) out vec3 v_WorldPos;\n";
		OutString += "layout(location = 1) out vec3 v_Normal;\n";
		OutString += "layout(location = 2) out vec2 v_TexCoord;\n";
		OutString += "layout(location = 3) out flat int v_EntityID;\n";
	}

	void ShaderStringFactory::CreateVInputDeclare(MaterialShaderPara& MSPara, uint32_t uiPassType, std::string& OutString)
	{
		Ref<VertexBuffer> vertexBuffer = MSPara.m_pStaticMesh->GetVertexBuffer();
		BufferLayout layout = vertexBuffer->GetLayout();
		uint32_t locationId = 0;
		for (auto iter : layout.GetElements())
		{
			OutString += "layout(location = " + std::to_string(locationId) + ")";
			OutString += " in ";
			switch (iter.Type)
			{
			case ShaderDataType::Float:
				OutString += "float ";
				break;
			case ShaderDataType::Float2:
				OutString += "vec2 ";
				break;
			case ShaderDataType::Float3:
				OutString += "vec3 ";
				break;
			case ShaderDataType::Float4:
				OutString += "vec4 ";
				break;
			case ShaderDataType::Int:
				OutString += "int ";
				break;
			}
			OutString += iter.Name + ";\n";
			++locationId;
		}
		//std::vector<VertexArray>
	}
	//------Input Declare------

	//------User Constant------
	void ShaderStringFactory::CreateVUserConstant(Ref<Shader> pShader, MaterialShaderPara& para, uint32_t& uiPassType, std::string& OutString)
	{
		//create uniform string
		if (uiPassType == RenderPass::RenderPassType::PT_MATERIAL)
		{
			CreateUserConstantModelViewProjectionMatrix(pShader, OutString);
		}
	}

	void ShaderStringFactory::CreateUserConstantModelViewProjectionMatrix(Ref<Shader> pShader, std::string& OutString)
	{
		OutString += "uniform mat4 " + ShaderStringFactory::m_ViewProjection + ";\n";
		OutString += "uniform mat4 " + ShaderStringFactory::m_Model + ";\n";
	}

	void ShaderStringFactory::CreateVFunction(MaterialShaderPara& MSPara, uint32_t uiPassType, std::string& OutString)
	{
		Ref<VertexBuffer> vertexBuffer = MSPara.m_pStaticMesh->GetVertexBuffer();
		BufferLayout layout = vertexBuffer->GetLayout();

		OutString += "void main(){\n";
		//Temp Variable, will be output the output variable
		OutString += "	vec4 WorldPos = vec4(0.0, 0.0, 0.0, 0.0);\n";
		OutString += "	vec3 Normal = vec3(0.0, 0.0, 0.0);\n";
		OutString += "	vec2 TexCoord = vec2(0.0, 0.0);\n";

		OutString += "	WorldPos = " + ShaderStringFactory::m_Model + " * " + "vec4(a_Pos, 1.0);\n";
		OutString += "	mat3 NormalMatrix = mat3(transpose(inverse(" + ShaderStringFactory::m_Model + ")));\n";
		OutString += "	Normal = NormalMatrix * a_Normal;\n";
		OutString += "	TexCoord = a_TexCoord;\n";
		
		//assign to output variable
		OutString += "	v_WorldPos = WorldPos.xyz;\n";
		OutString += "	v_Normal = Normal;\n";
		OutString += "	v_TexCoord = TexCoord;\n";
		OutString += "	v_EntityID = a_EntityID;\n";
		OutString += "	gl_Position = u_ViewProjection * WorldPos;\n";
		OutString += "}";
	}

	//-----User Constant-------

	bool ShaderStringFactory::CreateShaderString(Ref<Shader> shader, MaterialShaderPara& MSPara, uint32_t uiPassType, std::string& filePath)
	{
		{
			Ref<Material> pMaterial = MSPara.pMaterialInstance->GetMaterial();
			++m_CreateShaderNum;
			std::string VInclude;//TODO: common include
			std::string VDynamic;//TODO: bone material

			std::string VInputDeclare;//layout(location = 0) in vec3 a_Pos etc.
			std::string VOutputDeclare;//layout(location = 0) out vec3 v_Pos etc.

			std::string VUserConstantString;//uniform mat4 u_ViewProjection etc.
			std::string VFunctionString;//main function

			std::string shaderText;

			GetIncludeShader(VInclude);
			//uniform
			CreateVUserConstant(shader, MSPara, uiPassType, VUserConstantString);
			//Vertex Shader Input Declare
			CreateVInputDeclare(MSPara, uiPassType, VInputDeclare);
			//Vertex Shader Output Declare
			CreateVOutputDeclare(MSPara, uiPassType, VOutputDeclare);
			//main function
			CreateVFunction(MSPara, uiPassType, VFunctionString);

			shaderText = VInclude + "\n" + VInputDeclare + "\n" + VOutputDeclare + "\n" + VUserConstantString + "\n" + VFunctionString + "\n";

			//TODO:filename is temp use
			filePath = "assets/shaders/cache/" + MSPara.pMaterialInstance->GetMaterial()->GetMaterialName() + ".glsl";

			std::string PInclude;//TODO: common include
			std::string PDynamic;

			std::string PInputDeclare;
			std::string POutputDeclare;

			std::string PUserConstantString;
			std::string PFunctionString;
			
			GetPIncludeType(PInclude);

			CreatePInputDeclare(MSPara, uiPassType, PInputDeclare);

			CreatePOutputDeclare(MSPara, uiPassType, POutputDeclare);

			CreatePFunction(MSPara, uiPassType, PFunctionString);

			shaderText += PInclude + "\n" + PInputDeclare + "\n" + POutputDeclare + "\n" + PUserConstantString + "\n" + PFunctionString + "\n";

			std::ofstream out(filePath);
			out << shaderText;
			out.close();
		}
		

		return true;
	}

	void ShaderStringFactory::GetPIncludeType(std::string& OutString)
	{
		OutString += "#type fragment\n";
		OutString += "#version 450 core\n";

		std::string shaderInclude;
		std::string Temp;
		std::ifstream file("assets/shaders/test/shader.glsl");
		while (file.peek() != EOF)
		{
			std::getline(file, Temp);
			shaderInclude += Temp + "\n";
		}
		file.close();
		OutString += shaderInclude + "\n";
	}

	void ShaderStringFactory::CreatePInputDeclare(MaterialShaderPara& MSPara, uint32_t uiPassType, std::string& OutString)
	{
		std::string TempDeclare;
		OutString += "layout(location = 0) in vec3 v_WorldPos;\n";
		OutString += "layout(location = 1) in vec3 v_Normal;\n";
		OutString += "layout(location = 2) in vec2 v_TexCoord;\n";
		OutString += "layout(location = 3) in flat int v_EntityID;\n";
	}

	void ShaderStringFactory::CreatePOutputDeclare(MaterialShaderPara& MSPara, uint32_t uiPassType, std::string& OutString)
	{
		std::string TempDeclare;
		m_PSOutputColorValue = "OutColor";
		OutString += "layout(location = 0) out vec4 " + m_PSOutputColorValue + ";\n";
	}

	void ShaderStringFactory::CreatePFunction(MaterialShaderPara& MSPara, uint32_t uiPassType, std::string& OutString)
	{
		if (uiPassType == RenderPass::RenderPassType::PT_MATERIAL)
		{
			std::string FunctionBody;
			Ref<Material> pMaterial = MSPara.pMaterialInstance->GetMaterial();
			
			pMaterial->GetShaderTreeString(FunctionBody, MSPara, ShaderMainFunction::OST_MATERIAL, MSPara.uiPassId);

			OutString += "void main(){\n" + FunctionBody + "\n};\n";
		}
	}

	//------Local Position To World Position------
	void ShaderStringFactory::LocalToWorldPos(const std::string& LocalPos, std::string& OutString)
	{
		//OutString += ShaderStringFactory::m_WorldMatrix + " * " + 
	}
	//------Local Position To World Position------
}