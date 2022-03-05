#include "pxpch.h"

#include "Renderer3D.h"

#include "BaseType.h"

#include "Model.h"

#include "Pixel/Renderer/RenderCommand.h"

namespace Pixel {
	void LightPassStencil(int32_t LightIndex, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Framebuffer* geoPassFramebuffer, Framebuffer* LightPassFramebuffer);
	void LightPass(const EditorCamera& camera, glm::vec2& gScreenSize, int32_t LightIndex, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Framebuffer* geoPassFramebuffer, Framebuffer* LightPassFramebuffer);
	void test(std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights);
	//deferred shading geometry pass
	static Ref<Shader> m_GeoPass;

	//light pass
	static Ref<Shader> m_LightPass;

	//light stencil pass
	static Ref<Shader> m_LightStencilPass;

	//Point Light Volume(is a sphere)
	static Model SphereModel;

	static glm::mat4 viewProj;

	static Ref<Shader> m_TestShader;

	void Renderer3D::Init()
	{	
		m_GeoPass = Shader::Create("assets/shaders/DeferredShading/GeometryPass.glsl");
		//temporary is point light
		m_LightPass = Shader::Create("assets/shaders/DeferredShading/LightPass.glsl");
		m_LightStencilPass = Shader::Create("assets/shaders/DeferredShading/LightStencilPass.glsl");

		m_TestShader = Shader::Create("assets/shaders/DeferredShading/Test.glsl");

		//Light Point
		SphereModel = Model("assets/models/Sphere.obj");
	}

	void Renderer3D::DrawModel(const glm::mat4& transform, StaticMeshComponent& MeshComponent, MaterialComponent& Material, int EntityID)
	{
		std::vector<Ref<Texture2D>> MaterialTextures;
		MaterialTextures.push_back(Material.Albedo);
		MaterialTextures.push_back(Material.NormalMap);
		MaterialTextures.push_back(Material.Roughness);
		MaterialTextures.push_back(Material.Metallic);
		MaterialTextures.push_back(Material.Emissive);

 		MeshComponent.mesh.Draw(transform, m_GeoPass, MaterialTextures, EntityID);
	}

	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		
	}

	void Renderer3D::BeginScene(const EditorCamera& camera, Framebuffer* geometryFramebuffer)
	{
		viewProj = camera.GetViewProjection();
		geometryFramebuffer->Bind();
		m_GeoPass->Bind();

		//open depth test and depth write
		RenderCommand::DepthTest(1);
		RenderCommand::DepthMask(1);
		//clear color buffer and depth buffer
		RenderCommand::SetClearColor({0.0f, 0.0f, 0.0f, 0.0f});
		RenderCommand::Clear();
		//set all pixel's entity id to -1
		geometryFramebuffer->ClearAttachment(4, -1);

		//close stencil test
		RenderCommand::StencilTest(0);
		RenderCommand::ClearStencil();
		
		m_GeoPass->SetMat4("u_ViewProjection", viewProj);

		/*-------------------------------------------------------
		 middle call the DrawModel function to write the gbuffer
		--------------------------------------------------------*/	
	}

	void Renderer3D::EndScene(const EditorCamera& camera, glm::vec2& gScreenSize, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Framebuffer* geoPassFramebuffer, Framebuffer* LightPassFramebuffer)
	{
		geoPassFramebuffer->UnBind();
		RenderCommand::DepthTest(0);
		RenderCommand::DepthMask(0);

		/*--------------------------------------------
		----------------Light Pass--------------------
		----------------------------------------------*/

		LightPassFramebuffer->Bind();		
		RenderCommand::Clear();
		LightPassFramebuffer->SetDepthAttachmentRendererID(geoPassFramebuffer->GetDepthAttachmentRendererID());
		//open stencil
		RenderCommand::StencilTest(1);
		for (int32_t i = 0; i < Trans.size(); ++i)
		{
			LightPassStencil(i, Trans, Lights, geoPassFramebuffer, LightPassFramebuffer);
			LightPass(camera, gScreenSize, i, Trans, Lights, geoPassFramebuffer, LightPassFramebuffer);
			//test(Trans, Lights);
		}

		RenderCommand::StencilTest(0);
		LightPassFramebuffer->UnBind();
	}

	void test(std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights)
	{
		m_TestShader->Bind();
		RenderCommand::Blend(1);
		//关闭深度测试
		RenderCommand::DepthTest(0);
		//关闭模板测试
		RenderCommand::StencilTest(0);

		RenderCommand::Cull(1);
		RenderCommand::Cull(1);

		for (int32_t i = 0; i < Trans.size(); ++i)
		{
			m_TestShader->SetMat4("u_Model", Trans[i].GetTransform());
			m_TestShader->SetMat4("u_ViewProjection", viewProj);
			SphereModel.Draw();
		}

		RenderCommand::DepthTest(1);
		RenderCommand::StencilTest(1);
		RenderCommand::Blend(0);
		RenderCommand::Cull(0);
		m_TestShader->Unbind();
	}

	static void LightPassStencil(int32_t LightIndex, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Framebuffer* geoPassFramebuffer, Framebuffer* LightPassFramebuffer)
	{
		/*----------------------------------------------
		----Render Volume Sphere Stencil Value Buffer---
		------------------------------------------------*/
		LightPassFramebuffer->CloseColorAttachmentDraw();
		//bind the geometryFramebuffer's depth buffer	
		m_LightStencilPass->Bind();
		
		RenderCommand::DepthTest(1);
		//Close Cull
		RenderCommand::Cull(0);

		//clear stencil
		RenderCommand::ClearStencil();

		//Set Stencil Compare Func
		RenderCommand::SetStencilFunc(StencilFunc::ALWAYS, 0, 0);
		//Set Stencil Operator Func
		
		//back increment
		RenderCommand::SetFrontOrBackStencilOp(0, StencilOp::KEEP, StencilOp::INCREMENT, StencilOp::KEEP);
		//front decrement
		RenderCommand::SetFrontOrBackStencilOp(1, StencilOp::KEEP, StencilOp::DECREMENT, StencilOp::KEEP);
		
		//according to the light strength to calculate the new scale
		float radius = Lights[LightIndex].GetSphereLightVolumeRadius();
		Trans[LightIndex].SetScale(glm::vec3(radius));

		m_LightStencilPass->SetMat4("u_ViewProjection", viewProj);
		m_LightStencilPass->SetMat4("u_Model", Trans[LightIndex].GetTransform());

		SphereModel.Draw();

		RenderCommand::DepthTest(0);
	}

	static void LightPass(const EditorCamera& camera, glm::vec2& gScreenSize, int32_t LightIndex, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Framebuffer* geoPassFramebuffer, Framebuffer* LightPassFramebuffer)
	{
		m_LightPass->Bind();
		LightPassFramebuffer->SetColorAttachmentDraw(0);
		//not equal stecil buff already exist value, then raster pixel
		RenderCommand::SetStencilFunc(StencilFunc::NOTEQUAL, 0, 0xff);

		//close depth test
		RenderCommand::DepthTest(0);
		//open blend
		RenderCommand::Blend(1);

		//cull front
		RenderCommand::Cull(1);
		RenderCommand::CullFrontOrBack(1);

		//Bind Texture
		m_LightPass->SetInt("g_Position", 0);
		RenderCommand::BindTexture(0, geoPassFramebuffer->GetColorAttachmentRendererID(0));
		m_LightPass->SetInt("g_Normal", 1);
		RenderCommand::BindTexture(1, geoPassFramebuffer->GetColorAttachmentRendererID(1));
		m_LightPass->SetInt("g_Albedo", 2);
		RenderCommand::BindTexture(2, geoPassFramebuffer->GetColorAttachmentRendererID(2));
		m_LightPass->SetInt("g_RoughnessMetallicEmissive", 3);
		RenderCommand::BindTexture(3, geoPassFramebuffer->GetColorAttachmentRendererID(3));
		//Bind Texture

		m_LightPass->SetMat4("u_ViewProjection", viewProj);
		m_LightPass->SetMat4("u_Model", Trans[LightIndex].GetTransform());

		m_LightPass->SetFloat2("gScreenSize", gScreenSize);
		m_LightPass->SetFloat3("camPos", camera.GetPosition());

		//Set Light Attribute
		m_LightPass->SetFloat3("light.position", Trans[LightIndex].Translation);
		m_LightPass->SetFloat3("light.color", Lights[LightIndex].color);
		m_LightPass->SetFloat("light.constant", Lights[LightIndex].constant);
		m_LightPass->SetFloat("light.linear", Lights[LightIndex].linear);
		m_LightPass->SetFloat("light.quadratic", Lights[LightIndex].quadratic);

		SphereModel.Draw();
		//cull back
		RenderCommand::CullFrontOrBack(0);
		//close blend
		RenderCommand::Blend(0);
		RenderCommand::DepthTest(1);
		m_LightPass->Unbind();
	}
}