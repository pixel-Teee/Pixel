#pragma once

#include "Pixel/Renderer/Shader.h"

namespace Pixel {
	struct MaterialShaderPara;
	class ShaderStringFactory
	{
	public:
		//give the variable one id, variable name + id
		static int32_t m_ShaderValueIndex;

		static uint32_t m_CreateShaderNum;

		static std::string m_Model;
		static std::string m_ViewProjection;

		static std::string m_WorldPos;
		static std::string m_WorldNormal;

		static std::string m_PSInputLocalNormal;

		static void Init();

		//------Create Include Shader-------
		static void GetIncludeShader(std::string& OutString);
		//------Create Include Shader------

		//------Create Vertex Shader Input Declare-------
		//static void CreateInputDeclarePosition();
		static void CreateVOutputDeclare(MaterialShaderPara& MSPara, uint32_t uiPassType, std::string& OutString);
		static void CreateVInputDeclare(MaterialShaderPara& MSPara, uint32_t uiPassType, std::string& OutString);
		//------Create Vertex Shader Input Declare-------

		//------Create Uniform string-------
		//first and second parameter for dx
		static void CreateVUserConstant(Ref<Shader> pShader, MaterialShaderPara& para, uint32_t& uiPassType, std::string& OutString);

		static void CreateUserConstantModelViewProjectionMatrix(Ref<Shader> pShader, std::string& OutString);
		//------Create Uniform string-------

		//------Create MainFunction------
		static void CreateVFunction(MaterialShaderPara& MSPara, uint32_t uiPassType, std::string& OutString);
		//------Create MainFunction------

		//Create Shader
		static bool CreateShaderString(Ref<Shader> shader, MaterialShaderPara& MSPara, uint32_t uiPassType, std::string& filePath);

		//-------Create PInclude Type------
		static void GetPIncludeType(std::string& OutString);
		//-------Create PInclude Type------

		//------Create Pixel Shader Input Declare------
		static void CreatePInputDeclare(MaterialShaderPara& MSPara, uint32_t uiPassType, std::string& OutString);
		//------Create Pixel Shader Input Declare------

		//------Create Pixel Shader Output Declare------
		static void CreatePOutputDeclare(MaterialShaderPara& MSPara, uint32_t uiPassType, std::string& OutString);
		//------Create Pixel Shader Output Declare------

		//------Create Pixel Function------
		static void CreatePFunction(MaterialShaderPara& MSPara, uint32_t uiPassType, std::string& OutString);
		//-------Create Pixel Function------

		//------Local Position To World Position------
		static void LocalToWorldPos(const std::string& LocalPos, std::string& OutString);
		//------Local Position To World Position------
	};
}