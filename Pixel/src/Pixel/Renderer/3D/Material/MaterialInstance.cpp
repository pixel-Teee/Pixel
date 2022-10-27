#include "pxpch.h"

#include "MaterialInstance.h"
#include "Material.h"
#include "Pixel/Asset/AssetManager.h"

namespace Pixel {
	MaterialInstance::MaterialInstance()
	{

	}

	MaterialInstance::~MaterialInstance()
	{

	}

	void MaterialInstance::SetPixelShaderValue(const std::string& name, void* fValue, uint32_t count)
	{
		if (m_pMaterial != nullptr)
		{
			PX_CORE_ASSERT(m_PSShaderCustomValue.size() != m_pMaterial->m_PSShaderCustomValue.size(), "material instance is not match with material!");
			PX_CORE_ASSERT(m_PSShaderCustomTexture.size() != m_pMaterial->m_PSShaderCustomTexture.size(), "material instance is not match with material!");

			float* value = (float*)fValue;

			for (size_t i = 0; i < m_PSShaderCustomValue.size(); ++i)
			{
				if (m_PSShaderCustomValue[i]->ConstValueName == name)
				{
					PX_CORE_ASSERT(count == m_PSShaderCustomValue[i]->m_Values.size(), "count don't equal to material parameter size!");
					for (size_t j = 0; j < count; ++j)
					{
						m_PSShaderCustomValue[i]->m_Values[j] = value[j];
					}
					break;
				}
			}
		}
	}

	void MaterialInstance::SetPixelShaderTexture(const std::string& name, Ref<Texture2D> pTexture)
	{
		if (m_pMaterial != nullptr)
		{
			PX_CORE_ASSERT(m_PSShaderCustomValue.size() != m_pMaterial->m_PSShaderCustomValue.size(), "material instance is not match with material!");
			PX_CORE_ASSERT(m_PSShaderCustomTexture.size() != m_pMaterial->m_PSShaderCustomTexture.size(), "material instance is not match with material!");

			for (size_t i = 0; i < m_PSShaderCustomTexture.size(); ++i)
			{
				if (m_PSShaderCustomTexture[i]->ConstValueName == name)
				{
					m_PSShaderCustomTexture[i]->m_pTexture = pTexture;
					break;
				}
			}
		}
	}

	void MaterialInstance::ReConstructParameter(Ref<Material> pMaterial)
	{
		//------set parameter------
		m_PSShaderCustomValue.clear();
		m_PSShaderCustomValue.resize(pMaterial->m_PSShaderCustomValue.size());
		for (size_t i = 0; i < pMaterial->m_PSShaderCustomValue.size(); ++i)
		{
			m_PSShaderCustomValue[i] = CreateRef<CustomFloatValue>();
			//m_PSShaderCustomValue[i]->m_
			m_PSShaderCustomValue[i]->ConstValueName = pMaterial->m_PSShaderCustomValue[i]->ConstValueName;
			m_PSShaderCustomValue[i]->m_ValueType = pMaterial->m_PSShaderCustomValue[i]->m_ValueType;
			m_PSShaderCustomValue[i]->m_Values.resize(pMaterial->m_PSShaderCustomValue[i]->m_Values.size());
			for (size_t j = 0; j < m_PSShaderCustomValue[i]->m_Values.size(); ++j)
			{
				m_PSShaderCustomValue[i]->m_Values[j] = pMaterial->m_PSShaderCustomValue[i]->m_Values[j];//assign
			}
		}

		m_PSShaderCustomTexture.clear();
		m_PSShaderCustomTexture.resize(pMaterial->m_PSShaderCustomTexture.size());
		for (size_t i = 0; i < pMaterial->m_PSShaderCustomTexture.size(); ++i)
		{
			m_PSShaderCustomTexture[i] = CreateRef<CustomTexture2D>();
			m_PSShaderCustomTexture[i]->ConstValueName = pMaterial->m_PSShaderCustomTexture[i]->ConstValueName;
			m_PSShaderCustomTexture[i]->m_VirtualPath = pMaterial->m_PSShaderCustomTexture[i]->m_VirtualPath;
			m_PSShaderCustomTexture[i]->m_pTexture = AssetManager::GetSingleton().GetTexture(m_PSShaderCustomTexture[i]->m_VirtualPath);
		}
		//------set parameter------
	}

	void MaterialInstance::SetMaterial(std::string& virtualPath)
	{
		m_MaterialPath = virtualPath;
		//from the asset manager to get the material
		Ref<Material> pMaterial = AssetManager::GetSingleton().GetTestMaterial(virtualPath);

		m_pMaterial = pMaterial;

		PX_CORE_ASSERT(pMaterial != nullptr, "couldn't find material");

		//------set parameter------
		m_PSShaderCustomValue.clear();
		m_PSShaderCustomValue.resize(pMaterial->m_PSShaderCustomValue.size());
		for (size_t i = 0; i < pMaterial->m_PSShaderCustomValue.size(); ++i)
		{
			m_PSShaderCustomValue[i] = CreateRef<CustomFloatValue>();
			//m_PSShaderCustomValue[i]->m_
			m_PSShaderCustomValue[i]->ConstValueName = pMaterial->m_PSShaderCustomValue[i]->ConstValueName;
			m_PSShaderCustomValue[i]->m_ValueType = pMaterial->m_PSShaderCustomValue[i]->m_ValueType;
			m_PSShaderCustomValue[i]->m_Values.resize(pMaterial->m_PSShaderCustomValue[i]->m_Values.size());
			for (size_t j = 0; j < m_PSShaderCustomValue[i]->m_Values.size(); ++j)
			{
				m_PSShaderCustomValue[i]->m_Values[j] = pMaterial->m_PSShaderCustomValue[i]->m_Values[j];//assign
			}
		}

		m_PSShaderCustomTexture.clear();
		m_PSShaderCustomTexture.resize(pMaterial->m_PSShaderCustomTexture.size());
		for (size_t i = 0; i < pMaterial->m_PSShaderCustomTexture.size(); ++i)
		{
			m_PSShaderCustomTexture[i] = CreateRef<CustomTexture2D>();
			m_PSShaderCustomTexture[i]->ConstValueName = pMaterial->m_PSShaderCustomTexture[i]->ConstValueName;
			m_PSShaderCustomTexture[i]->m_VirtualPath = pMaterial->m_PSShaderCustomTexture[i]->m_VirtualPath;
			m_PSShaderCustomTexture[i]->m_pTexture = AssetManager::GetSingleton().GetTexture(m_PSShaderCustomTexture[i]->m_VirtualPath);
		}
		//------set parameter------
	}

	std::string MaterialInstance::GetMaterialPath()
	{
		return m_MaterialPath;
	}

	Ref<Material> MaterialInstance::GetMaterial()
	{
		return m_pMaterial;
	}

	void MaterialInstance::SetMaterial(Ref<Material> pMaterial)
	{
		m_pMaterial = pMaterial;

		PX_CORE_ASSERT(pMaterial != nullptr, "couldn't find material");

		//------set parameter------
		m_PSShaderCustomValue.clear();
		m_PSShaderCustomValue.resize(pMaterial->m_PSShaderCustomValue.size());
		for (size_t i = 0; i < pMaterial->m_PSShaderCustomValue.size(); ++i)
		{
			m_PSShaderCustomValue[i] = CreateRef<CustomFloatValue>();
			//m_PSShaderCustomValue[i]->m_
			m_PSShaderCustomValue[i]->ConstValueName = pMaterial->m_PSShaderCustomValue[i]->ConstValueName;
			m_PSShaderCustomValue[i]->m_ValueType = pMaterial->m_PSShaderCustomValue[i]->m_ValueType;
			m_PSShaderCustomValue[i]->m_Values.resize(pMaterial->m_PSShaderCustomValue[i]->m_Values.size());
			for (size_t j = 0; j < m_PSShaderCustomValue[i]->m_Values.size(); ++j)
			{
				m_PSShaderCustomValue[i]->m_Values[j] = pMaterial->m_PSShaderCustomValue[i]->m_Values[j];//assign
			}
		}

		m_PSShaderCustomTexture.clear();
		m_PSShaderCustomTexture.resize(pMaterial->m_PSShaderCustomTexture.size());
		for (size_t i = 0; i < pMaterial->m_PSShaderCustomTexture.size(); ++i)
		{
			m_PSShaderCustomTexture[i] = CreateRef<CustomTexture2D>();
			m_PSShaderCustomTexture[i]->ConstValueName = pMaterial->m_PSShaderCustomTexture[i]->ConstValueName;
			m_PSShaderCustomTexture[i]->m_VirtualPath = pMaterial->m_PSShaderCustomTexture[i]->m_VirtualPath;
			m_PSShaderCustomTexture[i]->m_pTexture = AssetManager::GetSingleton().GetTexture(m_PSShaderCustomTexture[i]->m_VirtualPath);
		}
		//------set parameter------
	}
}