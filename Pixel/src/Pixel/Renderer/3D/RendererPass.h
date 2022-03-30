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
			PT_MATERIAL, //material light
			PT_MAX
		};

		virtual ~RenderPass() = 0;
	protected:
		RenderPass();
		Ref<StaticMesh> m_pMesh; //current render mesh
		Ref<Camera> m_pCamera; //current camera
		RenderPassType m_uiPassId; //current pass id

		Ref<Shader> m_pShader; //current shader
		std::string m_ShaderName; //current use shader name, to find shader in shader library

		Ref<MaterialInstance> m_pMaterialInstance; //current use material instance

		MaterialShaderPara m_MSPara; //current use render parameter

	public:
		virtual bool Draw() = 0;
		virtual RenderPassType GetPassType() = 0;

		void SetShader(Ref<Shader> pShader)
		{
			m_pShader = pShader;
		}

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

		bool GetShader(MaterialShaderPara& MSPara, ShaderLibrary& ShaderLibrary, const std::string& Name);
	};
}
