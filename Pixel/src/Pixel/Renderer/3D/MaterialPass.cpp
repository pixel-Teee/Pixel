#include "pxpch.h"
#include "MaterialPass.h"

namespace Pixel
{
	
	MaterialPass::MaterialPass()
	{
		m_uiPassId = RenderPass::PT_MATERIAL;
	}

	MaterialPass::~MaterialPass()
	{

	}

	bool MaterialPass::Draw()
	{
		//TODO:light
		if (!m_pCamera || !m_pMesh || !m_pMaterialInstance)
		{
			return false;
		}

		Ref<Material> pMaterial = m_pMaterialInstance->GetMaterial();
		if (!pMaterial)
		{
			return false;
		}

		m_MSPara.pCamera = m_pCamera;
		m_MSPara.m_pStaticMesh = m_pMesh;
		m_MSPara.pMaterialInstance = m_pMaterialInstance;
		m_MSPara.uiPassId = m_uiPassId;

		//TODO:implement this
		GetShader(m_MSPara, ShaderLibrary::GetShaderLibrary(),"test");
	}

	Pixel::RenderPass::RenderPassType MaterialPass::GetPassType()
	{
		return m_uiPassId;
	}

}