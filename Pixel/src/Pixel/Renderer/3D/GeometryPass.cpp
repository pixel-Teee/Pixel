#include "pxpch.h"
#include "GeometryPass.h"

namespace Pixel
{

	GeometryPass::GeometryPass()
	{
		//geometry framebuffer
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA8,
		FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };

		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		
		m_GeometryFrameBuffer = Framebuffer::Create(fbSpec);

		m_uiPassId = RenderPassType::PT_GEOMETRY;
	}

	GeometryPass::~GeometryPass()
	{

	}

	bool GeometryPass::Resize(uint32_t width, uint32_t height)
	{
		//throw std::logic_error("The method or operation is not implemented.");
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA8,
		FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };

		fbSpec.Width = width;
		fbSpec.Height = height;

		m_GeometryFrameBuffer = Framebuffer::Create(fbSpec);
		return true;
	}

	bool GeometryPass::Draw()
	{
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

	RenderPass::RenderPassType GeometryPass::GetPassType()
	{
		return m_uiPassId;
	}
}