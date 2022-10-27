#include "pxpch.h"

//------my library------
#include "Material.h"
#include "ShaderMainFunction.h"
#include "InputNode.h"
#include "OutputNode.h"
#include "ConstFloatValue.h"
#include "TextureShaderFunction.h"
#include "Texture2DShaderFunction.h"
#include "Pixel/Asset/AssetManager.h"
//------my library------

namespace Pixel {

	Material::Material()
	{
		m_pShaderFunctionArray.clear();
		//m_pShaderMainFunction = CreateRef<ShaderMainFunction>();
		//m_pShaderFunctionArray.push_back(m_pShaderMainFunction);
	}

	Material::~Material()
	{

	}

	Material::Material(const std::string& materialName)
	{
		m_MaterialName = materialName;

		//------clear the shader function------
		m_pShaderFunctionArray.clear();
		//m_pShaderMainFunction = CreateRef<ShaderMainFunction>();
		//m_pShaderFunctionArray.push_back(m_pShaderMainFunction);
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
		return m_pShaderMainFunction->GetShaderTreeString(OutString);
	}

	void Material::SetMaterialName(const std::string& materialName)
	{
		m_MaterialName = materialName;
	}

	void Material::PostLink()
	{
		std::vector<Ref<InputNode>> m_InputPins;
		std::vector<Ref<OutputNode>> m_OutputPins;
		//get the pointer's link
		for (uint32_t i = 0; i < m_pShaderFunctionArray.size(); ++i)
		{
			for (uint32_t j = 0; j < m_pShaderFunctionArray[i]->GetInputNodeNum(); ++j)
			{
				m_InputPins.push_back(m_pShaderFunctionArray[i]->GetInputNode(j));
				m_pShaderFunctionArray[i]->GetInputNode(j)->m_pOwner = m_pShaderFunctionArray[i];
			}

			for (uint32_t j = 0; j < m_pShaderFunctionArray[i]->GetOutputNodeNum(); ++j)
			{
				m_OutputPins.push_back(m_pShaderFunctionArray[i]->GetOutputNode(j));
				m_pShaderFunctionArray[i]->GetOutputNode(j)->m_pOwner = m_pShaderFunctionArray[i];
			}
		}

		for (uint32_t i = 0; i < m_pShaderMainFunction->GetInputNodeNum(); ++i)
		{
			m_InputPins.push_back(m_pShaderMainFunction->GetInputNode(i));
		}

		//link
		for (auto& item : m_Links)
		{
			uint32_t InputPinId = item.x;
			uint32_t OutputPinId = item.y;

			Ref<InputNode> InputPin;
			Ref<OutputNode> OutputPin;

			for (size_t i = 0; i < m_InputPins.size(); ++i)
			{
				if (m_InputPins[i]->m_id == InputPinId)
					InputPin = m_InputPins[i];
			}

			for (size_t i = 0; i < m_OutputPins.size(); ++i)
			{
				if (m_OutputPins[i]->m_id == OutputPinId)
					OutputPin = m_OutputPins[i];
			}

			InputPin->Connection(OutputPin);
		}

		for (size_t i = 0; i < m_pShaderFunctionArray.size(); ++i)
		{
			if (m_pShaderFunctionArray[i]->GetFunctioNodeId() == 1)
			{
				m_pShaderMainFunction = std::static_pointer_cast<ShaderMainFunction>(m_pShaderFunctionArray[i]);
			}
		}

		//------link all parameters------
		LinkAllParameters();
		//------link all parameters------
	}

	void Material::CreateConstValueDeclare(std::string& OutString)
	{
		for (size_t i = 0; i < m_pShaderFunctionArray.size(); ++i)
		{
			rttr::type tempType = rttr::type::get(*m_pShaderFunctionArray[i]);

			if (!tempType.is_derived_from<ConstValue>() || !m_pShaderFunctionArray[i]->m_bIsVisited)
				continue;

			Ref<ConstValue> pTemp = std::static_pointer_cast<ConstValue>(m_pShaderFunctionArray[i]);

			pTemp->GetDeclareString(OutString);
		}
	}

	void Material::CreateTextureDeclare(std::string& OutString, uint32_t registerId)
	{
		m_textures.clear();
		for (size_t i = 0; i < m_pShaderFunctionArray.size(); ++i)
		{
			rttr::type tempType = rttr::type::get(*m_pShaderFunctionArray[i]);

			if (!tempType.is_derived_from<TextureShaderFunction>() || !m_pShaderFunctionArray[i]->m_bIsVisited)
				continue;

			Ref<TextureShaderFunction> pTemp = std::static_pointer_cast<TextureShaderFunction>(m_pShaderFunctionArray[i]);

			pTemp->GetDeclareString(OutString, registerId);
			++registerId;

			//record some reflection information
			m_textures.push_back(pTemp->m_ShowName);
		}
	}

	void Material::LinkAllParameters()
	{
		//create material parameter
		m_PSShaderCustomValue.clear();
		//interms of the shader function array to create custom float value and custom texture 2D
		for (size_t i = 0; i < m_pShaderFunctionArray.size(); ++i)
		{
			if (rttr::type::get(*m_pShaderFunctionArray[i]) == rttr::type::get<ConstFloatValue>())
			{
				Ref<ConstFloatValue> pConstFloatValue = std::static_pointer_cast<ConstFloatValue>(m_pShaderFunctionArray[i]);

				if (!pConstFloatValue->m_bIsCustom)
					continue;

				Ref<CustomFloatValue> pCustomFloatValue = CreateRef<CustomFloatValue>();
				pCustomFloatValue->ConstValueName = m_pShaderFunctionArray[i]->m_ShowName;
				pCustomFloatValue->m_Values.resize(pConstFloatValue->m_valueNumber);
				pCustomFloatValue->m_ValueType = (ValueType)(pConstFloatValue->m_valueNumber - 1);
				m_PSShaderCustomValue.push_back(pCustomFloatValue);
				
				for (size_t j = 0; j < pCustomFloatValue->m_Values.size(); ++j)
				{
					pCustomFloatValue->m_Values[j] = pConstFloatValue->m_Value[j];
				}
			}
		}

		m_PSShaderCustomTexture.clear();
		for (size_t i = 0; i < m_pShaderFunctionArray.size(); ++i)
		{
			if (rttr::type::get(*m_pShaderFunctionArray[i]) == rttr::type::get<Texture2DShaderFunction>())
			{
				Ref<Texture2DShaderFunction> pTexture2DShaderFunction = std::static_pointer_cast<Texture2DShaderFunction>(m_pShaderFunctionArray[i]);
				
				Ref<CustomTexture2D> pCustomTexture = CreateRef<CustomTexture2D>();
				pCustomTexture->ConstValueName = m_pShaderFunctionArray[i]->m_ShowName;
				
				//TODO:need to some information
				pCustomTexture->m_VirtualPath = pTexture2DShaderFunction->m_TextureVirtualPath;
				pCustomTexture->m_pTexture = AssetManager::GetSingleton().GetTexture(pCustomTexture->m_VirtualPath);//get texture
				m_PSShaderCustomTexture.push_back(pCustomTexture);
			}
		}
	}

}