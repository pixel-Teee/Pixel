#include "pxpch.h"
#include "Material.h"
#include "ShaderStringFactory.h"
#include "Pixel/Renderer/3D/ShaderPbrFunction.h"
#include "Pixel/Renderer/3D/MaterialPass.h"

namespace Pixel {
	void MaterialInterface::Create()
	{

	}

	//------Param------
	MaterialCustomPara::~MaterialCustomPara()
	{

	}
	CustomFloatValue::CustomFloatValue()
	{

	}

	CustomFloatValue::~CustomFloatValue()
	{

	}

	CustomTexSampler::CustomTexSampler()
	{

	}

	CustomTexSampler::~CustomTexSampler()
	{

	}
	//------Param------

	//------Material------
	Material::Material()
	{

	}

	Material::~Material()
	{

	}

	Material::Material(const std::string& showName, uint32_t uiMUT)
	{
		m_ShowName = showName;
		m_pShaderFunctionArray.clear();
		m_pShaderMainFunction.clear();
		
		if (uiMUT == MUT_PBR)
		{
			Ref<ShaderPbrFunction> pShaderPbrFunction = CreateRef<ShaderPbrFunction>();
			pShaderPbrFunction->Init();//important
			m_pShaderMainFunction.push_back(pShaderPbrFunction);
		}

		m_pPass[RenderPass::PT_MATERIAL] = CreateRef<MaterialPass>();
	}
	
	Ref<ShaderMainFunction> Material::GetMainFunction(uint32_t uiPassId)
	{
		return m_pShaderMainFunction[uiPassId];
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
		auto iter = find(m_pShaderFunctionArray.begin(), m_pShaderFunctionArray.end(), pShaderFunction);
		if (iter != m_pShaderFunctionArray.end())
		{
			m_pShaderFunctionArray.erase(iter);
		}
	}

	bool Material::GetShaderTreeString(std::string& OutString, MaterialShaderPara& MSPara, uint32_t uiOST, uint32_t uiPassId)
	{
		//temp variable id
		ShaderStringFactory::m_ShaderValueIndex = 0;
		for (uint32_t i = 0; i < m_pShaderFunctionArray.size(); ++i)
		{
			//TODO: implement ResetInShaderName
			//m_pShaderFunctionArray[i]->ResetInShaderName();
		}

		//current PassId
		return (std::static_pointer_cast<ShaderMainFunction>(m_pShaderMainFunction[uiPassId]))->GetShaderTreeString(OutString, uiOST);
	}

	void Material::SetBlendState(BlendState blendstate, uint32_t uiPassId)
	{

	}

	void Material::SetStencilState(StencilState stencilstate, uint32_t uiPassId)
	{

	}

	//------Material------

	//------Material Instance------
	MaterialInstance::MaterialInstance(Ref<Material> pMaterial)
	{
		if (pMaterial == nullptr)
			PIXEL_CORE_ERROR("material is null!");
		else
			m_pMaterial = pMaterial;
	}

	MaterialInstance::MaterialInstance()
	{

	}
	//------Material Instance------

	//------Pixel Shader------

	void MaterialInstance::SetPSShaderValue(Ref<Shader> pPSShader)
	{
		if (pPSShader == nullptr)
		{
			return;
		}

		pPSShader->Bind();

		for (uint32_t i = 0; i < m_PSShaderCustomValue.size(); ++i)
		{
			switch (m_PSShaderCustomValue[i].m_valueType)
			{
			case CustomFloatValue::VT_1:
			{
				pPSShader->SetFloat(m_PSShaderCustomValue[i].ConstValueName, m_PSShaderCustomValue[i].Value[0]);
				break;
			}
			case CustomFloatValue::VT_2:
			{
				glm::vec2 v(m_PSShaderCustomValue[i].Value[0], m_PSShaderCustomValue[i].Value[1]);
				pPSShader->SetFloat2(m_PSShaderCustomValue[i].ConstValueName, v);
				break;
			}
			case CustomFloatValue::VT_3:
			{
				glm::vec3 v(m_PSShaderCustomValue[i].Value[0], m_PSShaderCustomValue[i].Value[1], m_PSShaderCustomValue[i].Value[2]);
				pPSShader->SetFloat2(m_PSShaderCustomValue[i].ConstValueName, v);
				break;
			}
			case CustomFloatValue::VT_4:
			{
				glm::vec4 v(m_PSShaderCustomValue[i].Value[0], m_PSShaderCustomValue[i].Value[1], m_PSShaderCustomValue[i].Value[3],
					m_PSShaderCustomValue[i].Value[4]);
				pPSShader->SetFloat2(m_PSShaderCustomValue[i].ConstValueName, v);
				break;
			}
			case CustomFloatValue::VT_4x4:
			{
				static float _v[4][4];
				for (uint32_t i = 0; i < 4; ++i)
				{
					for (uint32_t j = 0; j < 4; ++j)
					{
						_v[i][j] = m_PSShaderCustomValue[i].Value[i * 4 + j];
					}
				}
				glm::mat4 v(_v[0][0], _v[0][1], _v[0][2], _v[0][3],
					_v[1][0], _v[1][1], _v[1][2], _v[1][3],
					_v[2][0], _v[2][1], _v[2][2], _v[2][3],
					_v[3][0], _v[3][1], _v[3][2], _v[3][3]);
				pPSShader->SetMat4(m_PSShaderCustomValue[i].ConstValueName, v);
				break;
			}
			}
		}

		//set texture
		for (uint32_t i = 0; i < m_PSShaderCustomTex.size(); ++i)
		{
			pPSShader->SetInt(m_PSShaderCustomTex[i].ConstValueName, i);
			m_PSShaderCustomTex[i].m_pTexture->Bind(i);
		}

		pPSShader->Unbind();
	}

	void MaterialInstance::SetPSShaderValue(const std::string& name, void* value)
	{
		for (uint32_t i = 0; i < m_PSShaderCustomValue.size(); ++i)
		{
			if (m_PSShaderCustomValue[i].ConstValueName == name)
			{
				switch (m_PSShaderCustomValue[i].m_valueType)
				{
				case CustomFloatValue::VT_1:
					m_PSShaderCustomValue[i].Value[0] = *(float*)value;
					break;
				case CustomFloatValue::VT_2:
					for (uint32_t j = 0; j < 2; ++j)
					{
						m_PSShaderCustomValue[i].Value[j] = *((float*)value + j);
					}
					break;
				case CustomFloatValue::VT_3:
					for (uint32_t j = 0; j < 3; ++j)
					{
						m_PSShaderCustomValue[i].Value[j] = *((float*)value + j);
					}
					break;
				case CustomFloatValue::VT_4:
					for (uint32_t j = 0; j < 4; ++j)
					{
						m_PSShaderCustomValue[i].Value[j] = *((float*)value + j);
					}
					break;
				case CustomFloatValue::VT_4x4:
					for (uint32_t j = 0; j < 16; ++j)
					{
						m_PSShaderCustomValue[i].Value[j] = *((float*)value + j);
					}
					break;
				}
				break;
			}
		}
	}

	void MaterialInstance::DeletePSShaderValue(const std::string& name)
	{
		for (auto iter = m_PSShaderCustomValue.begin(); iter != m_PSShaderCustomValue.end(); ++iter)
		{
			if ((*iter).ConstValueName == name)
			{
				m_PSShaderCustomValue.erase(iter);
				break;
			}
		}
	}

	void MaterialInstance::SetPSShaderTexture(const std::string& name, Ref<Texture2D> pTex)
	{
		for (auto iter = m_PSShaderCustomTex.begin(); iter != m_PSShaderCustomTex.end(); ++iter)
		{
			if ((*iter).ConstValueName == name)
			{
				iter->m_pTexture = pTex;
			}
		}
	}

	void MaterialInstance::DeletePSShaderTexture(const std::string& name)
	{
		for (auto iter = m_PSShaderCustomTex.begin(); iter != m_PSShaderCustomTex.end(); ++iter)
		{
			if ((*iter).ConstValueName == name)
			{
				m_PSShaderCustomTex.erase(iter);
				break;
			}
		}
	}

	//-----Vertex Shader------

	void MaterialInstance::SetVSShaderValue(Ref<Shader> pVSShader)
	{
		if (pVSShader == nullptr)
		{
			return;
		}

		pVSShader->Bind();

		for (uint32_t i = 0; i < m_VSShaderCustomValue.size(); ++i)
		{
			switch (m_VSShaderCustomValue[i].m_valueType)
			{
			case CustomFloatValue::VT_1:
			{
				pVSShader->SetFloat(m_VSShaderCustomValue[i].ConstValueName, m_VSShaderCustomValue[i].Value[0]);
				break;
			}
			case CustomFloatValue::VT_2:
			{
				glm::vec2 v(m_VSShaderCustomValue[i].Value[0], m_VSShaderCustomValue[i].Value[1]);
				pVSShader->SetFloat2(m_VSShaderCustomValue[i].ConstValueName, v);
				break;
			}
			case CustomFloatValue::VT_3:
			{
				glm::vec3 v(m_VSShaderCustomValue[i].Value[0], m_VSShaderCustomValue[i].Value[1], m_VSShaderCustomValue[i].Value[2]);
				pVSShader->SetFloat2(m_VSShaderCustomValue[i].ConstValueName, v);
				break;
			}
			case CustomFloatValue::VT_4:
			{
				glm::vec4 v(m_VSShaderCustomValue[i].Value[0], m_VSShaderCustomValue[i].Value[1], m_VSShaderCustomValue[i].Value[3],
					m_VSShaderCustomValue[i].Value[4]);
				pVSShader->SetFloat2(m_VSShaderCustomValue[i].ConstValueName, v);
				break;
			}
			case CustomFloatValue::VT_4x4:
			{
				static float _v[4][4];
				for (uint32_t i = 0; i < 4; ++i)
				{
					for (uint32_t j = 0; j < 4; ++j)
					{
						_v[i][j] = m_VSShaderCustomValue[i].Value[i * 4 + j];
					}
				}
				glm::mat4 v(_v[0][0], _v[0][1], _v[0][2], _v[0][3],
					_v[1][0], _v[1][1], _v[1][2], _v[1][3],
					_v[2][0], _v[2][1], _v[2][2], _v[2][3],
					_v[3][0], _v[3][1], _v[3][2], _v[3][3]);
				pVSShader->SetMat4(m_VSShaderCustomValue[i].ConstValueName, v);
				break;
			}
			}
		}

		//set texture
		for (uint32_t i = 0; i < m_VSShaderCustomTex.size(); ++i)
		{
			pVSShader->SetInt(m_VSShaderCustomTex[i].ConstValueName, i);
			m_VSShaderCustomTex[i].m_pTexture->Bind(i);
		}

		pVSShader->Unbind();
	}

	void MaterialInstance::SetVSShaderValue(const std::string& name, void* value)
	{
		for (uint32_t i = 0; i < m_VSShaderCustomValue.size(); ++i)
		{
			if (m_VSShaderCustomValue[i].ConstValueName == name)
			{
				switch (m_VSShaderCustomValue[i].m_valueType)
				{
				case CustomFloatValue::VT_1:
					m_VSShaderCustomValue[i].Value[0] = *(float*)value;
					break;
				case CustomFloatValue::VT_2:
					for (uint32_t j = 0; j < 2; ++j)
					{
						m_VSShaderCustomValue[i].Value[j] = *((float*)value + j);
					}
					break;
				case CustomFloatValue::VT_3:
					for (uint32_t j = 0; j < 3; ++j)
					{
						m_VSShaderCustomValue[i].Value[j] = *((float*)value + j);
					}
					break;
				case CustomFloatValue::VT_4:
					for (uint32_t j = 0; j < 4; ++j)
					{
						m_VSShaderCustomValue[i].Value[j] = *((float*)value + j);
					}
					break;
				case CustomFloatValue::VT_4x4:
					for (uint32_t j = 0; j < 16; ++j)
					{
						m_VSShaderCustomValue[i].Value[j] = *((float*)value + j);
					}
					break;
				}
				break;
			}
		}
	}

	void MaterialInstance::DeleteVSShaderValue(const std::string& name)
	{
		for (auto iter = m_VSShaderCustomValue.begin(); iter != m_VSShaderCustomValue.end(); ++iter)
		{
			if ((*iter).ConstValueName == name)
			{
				m_VSShaderCustomValue.erase(iter);
				break;
			}
		}
	}

	void MaterialInstance::SetVSShaderTexture(const std::string& name, Ref<Texture2D> pTex)
	{
		for (auto iter = m_VSShaderCustomTex.begin(); iter != m_VSShaderCustomTex.end(); ++iter)
		{
			if ((*iter).ConstValueName == name)
			{
				iter->m_pTexture = pTex;
			}
		}
	}

	void MaterialInstance::DeleteVSShaderTexture(const std::string& name)
	{
		for (auto iter = m_VSShaderCustomTex.begin(); iter != m_VSShaderCustomTex.end(); ++iter)
		{
			if ((*iter).ConstValueName == name)
			{
				m_VSShaderCustomTex.erase(iter);
			}
		}
	}

	MaterialInstance::~MaterialInstance()
	{

	}
	//------Material Instance------
}