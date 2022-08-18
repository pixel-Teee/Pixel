#include "pxpch.h"

//------my library------
#include "Material.h"
#include "ShaderMainFunction.h"
//------my library------

namespace Pixel {

	Material::Material()
	{

	}

	Material::~Material()
	{

	}

	Material::Material(const std::string& materialName)
	{
		m_MaterialName = materialName;

		//------clear the shader function------
		m_pShaderFunctionArray.clear();
		m_pShaderMainFunction = nullptr;
		//------clear the shader function------
	}

	Ref<ShaderMainFunction> Material::GetMainFunction()
	{
		return m_pShaderMainFunction;
	}

	std::vector<Ref<ShaderFunction>> Material::GetShaderFunction()
	{
		return m_pShaderFunctionArray;
	}

	void Material::AddShaderFunction(Ref<ShaderFunction> pShaderFunction)
	{
		if (pShaderFunction != nullptr)
		{
			m_pShaderFunctionArray.push_back(pShaderFunction);
		}
	}

	void Material::DeleteShaderFunction(Ref<ShaderFunction> pShaderFunction)
	{
		//from the vector to find the shader function, then delete it
		auto iter = find(m_pShaderFunctionArray.begin(), m_pShaderFunctionArray.end(), pShaderFunction);

		if (iter != m_pShaderFunctionArray.end())
		{
			m_pShaderFunctionArray.erase(iter);
		}
	}

	bool Material::GetShaderTreeString(std::string& OutString)
	{
		return std::static_pointer_cast<ShaderMainFunction>(m_pShaderMainFunction)->GetShaderTreeString(OutString);
	}

}