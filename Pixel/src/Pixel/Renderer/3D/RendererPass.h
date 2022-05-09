#pragma once

#include "Pixel/Renderer/Shader.h"
#include "Pixel/Renderer/Framebuffer.h"
#include "Pixel/Renderer/Camera.h"
#include "Pixel/Renderer/3D/StaticMesh.h"
#include "Pixel/Renderer/3D/Material.h"

namespace Pixel
{
	class RenderPass
	{
	public:
		enum RenderPassType
		{
			PT_GEOMETRY, //deferred shading geometry pass
			PT_STENCIL, //deferred shading stencil pass
			PT_LIGHT, //deferred shading light pass
			PT_MAX
		};

		virtual ~RenderPass() = 0;
	protected:
		RenderPass();

		Ref<StaticMesh> m_pMesh; //current render mesh
		Ref<Camera> m_pCamera; //current camera
		RenderPassType m_uiPassId; //current pass id

		Ref<ShaderLibrary::ShaderSet> m_pShaderSet;//shader set, include this pass to renderer
		Ref<ShaderKey> m_pShaderKey;//shader key, to find shader in pShaderSet

		Ref<MaterialInstance> m_pMaterialInstance; //current use material instance

		MaterialShaderPara m_MSPara; //current use render parameter

		BlendState m_blendstate;
		StencilState m_stencilstate;
	public:
		virtual bool Resize(uint32_t width, uint32_t height) = 0;
		virtual bool Draw() = 0;
		virtual RenderPassType GetPassType() = 0;

		//---set current member---

		void SetCamera(Ref<Camera> pCamera)
		{
			m_pCamera = pCamera;
		}

		void SetMaterialInstance(Ref<MaterialInstance> pMaterialInstance)
		{
			m_pMaterialInstance = pMaterialInstance;
		}

		void SetMesh(Ref<StaticMesh> pMesh)
		{
			m_pMesh = pMesh;
		}
		//---set current member---
		bool GetShader(MaterialShaderPara& MSPara, Ref<ShaderLibrary> shaderLibrary, std::string& Name);

		void SetBlendState(BlendState blendstate);
		void SetStencilState(StencilState stencilstate);
	};
}
