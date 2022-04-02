#include "pxpch.h"
#include "Pixel/Renderer/Shader.h"
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

		Ref<ShaderLibrary> pLibrary = ShaderResourceManager::GetMaterialShaderMap();
		//TODO:implement this
		//GetShader(m_MSPara, pLibrary, 0);

		//set shader to material
		if (!GetShader(m_MSPara, pLibrary, pMaterial->GetMaterialName()))
		{
			return false;
		}

		//set material instance's parameter to shader

		//draw
	}

	Pixel::RenderPass::RenderPassType MaterialPass::GetPassType()
	{
		return m_uiPassId;
	}

}