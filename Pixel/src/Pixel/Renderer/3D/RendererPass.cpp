#include "pxpch.h"

#include "RendererPass.h"
#include "ShaderStringFactory.h"
#include "Pixel/Renderer/Shader.h"

namespace Pixel
{

	RenderPass::~RenderPass()
	{

	}

	RenderPass::RenderPass()
	{
		m_pShaderKey = CreateRef<ShaderKey>();
	}

	bool RenderPass::GetShader(MaterialShaderPara& MSPara, Ref<ShaderLibrary> shaderLibrary, std::string& Name)
	{
		Ref<Shader> pShader = nullptr;

		m_pShaderSet = shaderLibrary->GetShaderSet(Name);

		//Create Shader Key, in terms of PassType and MSPara
		
		//Shader Key is static
		ShaderKey::SetMaterialShaderKey(m_pShaderKey, MSPara, GetPassType());

		//in terms of shader key, to find the shader

		if (m_pMaterialInstance->m_CurrShader[GetPassType()] == nullptr)
		{
			//in terms of shader key to find shader
			if (m_pShaderSet)
			{
				auto it = m_pShaderSet->find(m_pShaderKey);
				if (it != m_pShaderSet->end())
				{
					pShader = it->second;
				}
			}

			if (pShader == nullptr)
			{
				//create shader
				pShader = ShaderResourceManager::CreateShader(MSPara, GetPassType(), 0);

				if (pShader == nullptr)
					return false;

				//add to shader set
				if (m_pShaderSet)
				{
					m_pShaderSet->insert(std::make_pair(m_pShaderKey, pShader));
				}
			}

			m_pMaterialInstance->m_CurrShader[GetPassType()] = pShader;
		}
		
		return true;
	}

}