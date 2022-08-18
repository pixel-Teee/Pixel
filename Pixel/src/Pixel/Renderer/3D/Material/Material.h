#pragma once

#include "MaterialBase.h"

namespace Pixel {
	//------forward declare------
	class ShaderMainFunction;
	class ShaderFunction;
	//------forward declare------
	class Material : public MaterialBase
	{
	public:

		Material();

		virtual ~Material();

		Material(const std::string& materialName);

		Ref<ShaderMainFunction> GetMainFunction();

		std::vector<Ref<ShaderFunction>> GetShaderFunction();

		void AddShaderFunction(Ref<ShaderFunction> pShaderFunction);

		void DeleteShaderFunction(Ref<ShaderFunction> pShaderFunction);

		bool GetShaderTreeString(std::string& OutString);

		std::string& GetMaterialName() { return m_MaterialName; }

		std::string m_MaterialName;//material name

		//logic node
		//main light calculate shader
		Ref<ShaderMainFunction> m_pShaderMainFunction;

		//other shader function
		std::vector<Ref<ShaderFunction>> m_pShaderFunctionArray;
	};
}