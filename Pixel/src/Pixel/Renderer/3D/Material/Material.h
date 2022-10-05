#pragma once

#include "MaterialBase.h"

//TODO:reflection module need this header file
#include "ShaderFunction.h"

namespace Pixel {
	//------forward declare------
	class ShaderMainFunction;
	class ShaderFunction;
	class Shader;
	//------forward declare------

	class Meta(Enable) Material : public MaterialBase
	{
	public:
		Meta()
		Material();

		virtual ~Material();

		Meta()
		Material(const std::string& materialName);

		Ref<ShaderMainFunction> GetMainFunction();

		std::vector<Ref<ShaderFunction>> GetShaderFunction();

		void AddShaderFunction(Ref<ShaderFunction> pShaderFunction);

		void DeleteShaderFunction(Ref<ShaderFunction> pShaderFunction);

		bool GetShaderTreeString(std::string& OutString);

		//create const buffer value declare
		void CreateConstValueDeclare(std::string& OutString);

		//create texture value declare
		void CreateTextureDeclare(std::string & OutString, uint32_t registerId);

		std::string& GetMaterialName() { return m_MaterialName; }

		void SetMaterialName(const std::string& materialName);

		std::vector<glm::ivec2>& GetLinks() { return m_Links; }

		void LinkAllParameters();

		Meta()
		std::string m_MaterialName;//material name

		//------material's uninitialized pso index, will in terms of the model's vertex input layout to create complete pso------
		//Meta()
		bool dirty = false;

		//Meta()
		int32_t m_PsoIndex = -1;
		//------material's uninitialized pso index, will in terms of the model's vertex input layout to create complete pso------

		//logic node
		//main light calculate shader
		Ref<ShaderMainFunction> m_pShaderMainFunction;

		//other shader function
		Meta()
		std::vector<Ref<ShaderFunction>> m_pShaderFunctionArray;

		//std::map<uint32_t, uint32_t> m_Links;//input node <=> output node
		Meta()
		std::vector<glm::ivec2> m_Links;

		Ref<Shader> m_pVertexShader;

		Ref<Shader> m_pPixelShader;

		void PostLink();

		RTTR_ENABLE(MaterialBase)
		RTTR_REGISTRATION_FRIEND
	};
}