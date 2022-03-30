#include "pxpch.h"

#include "RendererPass.h"
#include "ShaderStringFactory.h"

namespace Pixel
{

	RenderPass::~RenderPass()
	{

	}

	RenderPass::RenderPass()
	{

	}

	//third parameter is shader name
	bool RenderPass::GetShader(MaterialShaderPara& MSPara, ShaderLibrary& shaderLibrary, const std::string& Name)
	{
		Ref<Shader> shader;

		if (m_pMaterialInstance->m_CurrShader == nullptr)
		{
			//ShaderStringFactory
			if (shaderLibrary.Exists(Name))
			{
				shader = shaderLibrary.Get(Name);
			}
			else
			{
				std::string filePath;
				ShaderStringFactory::CreateShaderString(shader, MSPara, GetPassType(), filePath);

				shader = Shader::Create(filePath);
				shaderLibrary.Add(shader);
			}
		}
		
		return true;
	}

}