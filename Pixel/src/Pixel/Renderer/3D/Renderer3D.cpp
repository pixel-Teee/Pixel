#include "pxpch.h"

#include "Renderer3D.h"

#include "BaseType.h"

#include "Model.h"

#include "Pixel/Renderer/RenderCommand.h"
#include "Pixel/Renderer/UniformBuffer.h"

#include <glm/gtc/type_ptr.hpp>

namespace Pixel {
	void LightPassStencil(int32_t LightIndex, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Ref<Framebuffer> geoPassFramebuffer, Ref<Framebuffer> LightPassFramebuffer);
	void LightPass(Camera& camera, TransformComponent& trans, glm::vec2& gScreenSize, int32_t LightIndex, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Ref<Framebuffer> geoPassFramebuffer, Ref<Framebuffer> LightPassFramebuffer);

	//Editor
	void LightPass(const EditorCamera& camera, glm::vec2& gScreenSize, int32_t LightIndex, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Ref<Framebuffer> geoPassFramebuffer, Ref<Framebuffer> LightPassFramebuffer);
	//void test(std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights);
	//deferred shading geometry pass
	static Ref<Shader> m_GeoPass;

	//light pass
	static Ref<Shader> m_LightPass;

	//light stencil pass
	static Ref<Shader> m_LightStencilPass;

	//Point Light Volume(is a sphere)
	static Model SphereModel;

	static glm::mat4 viewProj;

	//static Ref<Shader> m_TestShader;

	static Ref<UniformBuffer> m_MVPUuniformBuffer;
	static Ref<UniformBuffer> m_CameraUniformBuffer;
	static Ref<UniformBuffer> m_lightUniformBuffer;
	static std::vector<uint32_t> camTypeOffsets;
	static std::vector<uint32_t> lightTypeOffsets;

	static Ref<CubeMap> m_SkyBox;
	static Ref<Shader> m_SkyBoxShader;
	static Model m_Box;

	void Renderer3D::Init()
	{	
		m_GeoPass = Shader::Create("assets/shaders/DeferredShading/GeometryPass.glsl");
		//temporary is point light
		m_LightPass = Shader::Create("assets/shaders/DeferredShading/LightPass.glsl");
		m_LightStencilPass = Shader::Create("assets/shaders/DeferredShading/LightStencilPass.glsl");
		m_SkyBoxShader = Shader::Create("assets/shaders/SkyBox/SkyBox.glsl");
		//m_TestShader = Shader::Create("assets/shaders/DeferredShading/Test.glsl");

		//Light Point
		SphereModel = Model("assets/models/Sphere.obj");
		m_Box = Model("assets/models/Box.obj");

		std::vector<std::string> paths{"assets/textures/skybox/right.jpg",
		"assets/textures/skybox/left.jpg",
		"assets/textures/skybox/top.jpg",
		"assets/textures/skybox/bottom.jpg",
		"assets/textures/skybox/front.jpg",
		"assets/textures/skybox/back.jpg"};

		m_SkyBox = CubeMap::Create(paths);

		//bind to uniform block offset:64
		m_MVPUuniformBuffer = UniformBuffer::Create(0, 128, 0);

		Utils::CalculateTypeOffsetAndSize<16, glm::vec2, glm::vec3>(0, camTypeOffsets);
		m_CameraUniformBuffer = UniformBuffer::Create(0, camTypeOffsets.back() + sizeof(glm::vec3), 2);

		Utils::CalculateTypeOffsetAndSize<16, glm::vec3, glm::vec3, float, float, float>(0, lightTypeOffsets);
		m_lightUniformBuffer = UniformBuffer::Create(0, lightTypeOffsets.back() + sizeof(float), 1);
	}

	void Renderer3D::DrawModel(const glm::mat4& transform, StaticMeshComponent& MeshComponent, MaterialComponent& Material, int EntityID)
	{
		std::vector<Ref<Texture2D>> MaterialTextures;
		MaterialTextures.push_back(Material.Albedo);
		MaterialTextures.push_back(Material.NormalMap);
		MaterialTextures.push_back(Material.Roughness);
		MaterialTextures.push_back(Material.Metallic);
		MaterialTextures.push_back(Material.Emissive);

 		MeshComponent.mesh.Draw(transform, m_GeoPass, MaterialTextures, EntityID, m_MVPUuniformBuffer);
	}

	void Renderer3D::BeginScene(const Camera& camera, TransformComponent& transform, Ref<Framebuffer> geometryFramebuffer)
	{
		viewProj = camera.GetProjection() * glm::inverse(transform.GetTransform());
		geometryFramebuffer->Bind();
		m_GeoPass->Bind();

		//open depth test and depth write
		RenderCommand::DepthTest(1);
		RenderCommand::DepthMask(1);
		//clear color buffer and depth buffer
		RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
		RenderCommand::Clear();
		//set all pixel's entity id to -1
		geometryFramebuffer->ClearAttachment(4, -1);

		//close stencil test
		RenderCommand::StencilTest(0);
		RenderCommand::ClearStencil();

		//m_GeoPass->SetMat4("u_ViewProjection", viewProj);
		m_MVPUuniformBuffer->SetData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewProj));
		/*-------------------------------------------------------
		 middle call the DrawModel function to write the gbuffer
		--------------------------------------------------------*/
	}

	void Renderer3D::BeginScene(const EditorCamera& camera, Ref<Framebuffer> geometryFramebuffer)
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
		
		//m_GeoPass->SetMat4("u_ViewProjection", viewProj);
		m_MVPUuniformBuffer->SetData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewProj));
		/*-------------------------------------------------------
		 middle call the DrawModel function to write the gbuffer
		--------------------------------------------------------*/	
	}

	void Renderer3D::EndScene(const EditorCamera& camera, glm::vec2& gScreenSize, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Ref<Framebuffer> geoPassFramebuffer, Ref<Framebuffer> LightPassFramebuffer)
	{
		geoPassFramebuffer->UnBind();
		RenderCommand::DepthTest(0);
		RenderCommand::DepthMask(0);

		/*--------------------------------------------
		----------------Light Pass--------------------
		----------------------------------------------*/

		LightPassFramebuffer->Bind();
		//LightPassFramebuffer->SetColorAttachmentDraw(0);
		RenderCommand::Clear();

		BindReadFramebuffer(geoPassFramebuffer->GetRenderId());
		BindWriteFramebuffer(LightPassFramebuffer->GetRenderId());
		uint32_t width = LightPassFramebuffer->GetSpecification().Width;
		uint32_t height = LightPassFramebuffer->GetSpecification().Height;
		BlitDepthFramebuffer(width, height);

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

		/*BindReadFramebuffer(geoPassFramebuffer->GetRenderId());
		BindWriteFramebuffer(LightPassFramebuffer->GetRenderId());
		uint32_t width = LightPassFramebuffer->GetSpecification().Width;
		uint32_t height = LightPassFramebuffer->GetSpecification().Height;
		BlitDepthFramebuffer(width, height);*/
	}

	void Renderer3D::EndScene(Camera& camera, TransformComponent& trans, glm::vec2& gScreenSize, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Ref<Framebuffer> geoPassFramebuffer, Ref<Framebuffer> LightPassFramebuffer)
	{
		geoPassFramebuffer->UnBind();
		RenderCommand::DepthTest(0);
		RenderCommand::DepthMask(0);

		/*--------------------------------------------
		----------------Light Pass--------------------
		----------------------------------------------*/

		LightPassFramebuffer->Bind();
		//LightPassFramebuffer->SetColorAttachmentDraw(0);
		RenderCommand::Clear();

		BindReadFramebuffer(geoPassFramebuffer->GetRenderId());
		BindWriteFramebuffer(LightPassFramebuffer->GetRenderId());
		uint32_t width = LightPassFramebuffer->GetSpecification().Width;
		uint32_t height = LightPassFramebuffer->GetSpecification().Height;
		BlitDepthFramebuffer(width, height);
		//LightPassFramebuffer->SetDepthAttachmentRendererID(geoPassFramebuffer->GetDepthAttachmentRendererID());
		//open stencil
		RenderCommand::StencilTest(1);
		for (int32_t i = 0; i < Trans.size(); ++i)
		{
			LightPassStencil(i, Trans, Lights, geoPassFramebuffer, LightPassFramebuffer);
			LightPass(camera, trans, gScreenSize, i, Trans, Lights, geoPassFramebuffer, LightPassFramebuffer);
			//test(Trans, Lights);
		}

		RenderCommand::StencilTest(0);

		LightPassFramebuffer->UnBind();
	}

	Ref<CubeMap> Renderer3D::GetSkyBox()
	{
		return m_SkyBox;
	}

	void Renderer3D::DrawSkyBox(const EditorCamera& camera, Ref<Framebuffer> LightPassFramebuffer, Ref<Framebuffer> geoPassFramebuffer)
	{
		LightPassFramebuffer->Bind();
		LightPassFramebuffer->SetColorAttachmentDraw(0);
		RenderCommand::Cull(1);
		RenderCommand::CullFrontOrBack(1);
		/*BindReadFramebuffer(geoPassFramebuffer->GetRenderId());
		BindWriteFramebuffer(LightPassFramebuffer->GetRenderId());
		uint32_t width = LightPassFramebuffer->GetSpecification().Width;
		uint32_t height = LightPassFramebuffer->GetSpecification().Height;
		BlitDepthFramebuffer(width, height);*/

		RenderCommand::DepthFunc(DepthComp::LEQUAL);
		m_SkyBoxShader->Bind();

		glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glm::mat4 proj = camera.GetProjection();
		glm::mat4 viewProj = proj * view;

		m_MVPUuniformBuffer->SetData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewProj));

		m_SkyBoxShader->SetInt("SkyBox", 0);
		m_SkyBox->Bind();
		m_Box.Draw();

		RenderCommand::DepthFunc(DepthComp::LESS);
		RenderCommand::Cull(0);
		RenderCommand::CullFrontOrBack(0);
		LightPassFramebuffer->UnBind();
	}

	void Renderer3D::DrawSkyBox(Camera& camera, TransformComponent& trans, Ref<Framebuffer> LightPassFramebuffer, Ref<Framebuffer> geoPassFramebuffer)
	{
		LightPassFramebuffer->Bind();
		LightPassFramebuffer->SetColorAttachmentDraw(0);
		RenderCommand::Cull(1);
		RenderCommand::CullFrontOrBack(1);
		/*BindReadFramebuffer(geoPassFramebuffer->GetRenderId());
		BindWriteFramebuffer(LightPassFramebuffer->GetRenderId());
		uint32_t width = LightPassFramebuffer->GetSpecification().Width;
		uint32_t height = LightPassFramebuffer->GetSpecification().Height;
		BlitDepthFramebuffer(width, height);*/

		RenderCommand::DepthFunc(DepthComp::LEQUAL);
		m_SkyBoxShader->Bind();

		glm::mat4 view = glm::mat4(glm::mat3(trans.GetTransform()));
		glm::mat4 proj = camera.GetProjection();
		glm::mat4 viewProj = proj * view;

		m_MVPUuniformBuffer->SetData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewProj));

		m_SkyBoxShader->SetInt("SkyBox", 0);
		m_SkyBox->Bind();
		m_Box.Draw();

		RenderCommand::DepthFunc(DepthComp::LESS);
		RenderCommand::Cull(0);
		RenderCommand::CullFrontOrBack(0);
		LightPassFramebuffer->UnBind();
	}

	//void test(std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights)
	//{
	//	m_TestShader->Bind();
	//	RenderCommand::Blend(1);
	//	//关闭深度测试
	//	RenderCommand::DepthTest(0);
	//	//关闭模板测试
	//	RenderCommand::StencilTest(0);

	//	RenderCommand::Cull(1);
	//	RenderCommand::Cull(1);

	//	for (int32_t i = 0; i < Trans.size(); ++i)
	//	{
	//		//m_TestShader->SetMat4("u_Model", Trans[i].GetTransform());
	//		//m_TestShader->SetMat4("u_ViewProjection", viewProj);
	//		glm::mat4 lightTrans = Trans[i].GetTransform();
	//		m_MVPUuniformBuffer->SetData(0, sizeof(glm::mat4), glm::value_ptr(lightTrans));
	//		m_MVPUuniformBuffer->SetData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewProj));
	//		SphereModel.Draw();
	//	}

	//	RenderCommand::DepthTest(1);
	//	RenderCommand::StencilTest(1);
	//	RenderCommand::Blend(0);
	//	RenderCommand::Cull(0);
	//	m_TestShader->Unbind();
	//}

	static void LightPassStencil(int32_t LightIndex, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Ref<Framebuffer> geoPassFramebuffer, Ref<Framebuffer> LightPassFramebuffer)
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

		//m_LightStencilPass->SetMat4("u_ViewProjection", viewProj);
		//m_LightStencilPass->SetMat4("u_Model", Trans[LightIndex].GetTransform());
		glm::mat4 lightTrans = Trans[LightIndex].GetTransform();
		m_MVPUuniformBuffer->SetData(0, sizeof(glm::mat4), glm::value_ptr(lightTrans));
		m_MVPUuniformBuffer->SetData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewProj));

		SphereModel.Draw();

		RenderCommand::DepthTest(0);
	}

	static void LightPass(const EditorCamera& camera, glm::vec2& gScreenSize, int32_t LightIndex, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Ref<Framebuffer> geoPassFramebuffer, Ref<Framebuffer> LightPassFramebuffer)
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

		/*m_LightPass->SetMat4("u_ViewProjection", viewProj);
		m_LightPass->SetMat4("u_Model", Trans[LightIndex].GetTransform());*/
		glm::mat4 lightTrans = Trans[LightIndex].GetTransform();
		m_MVPUuniformBuffer->SetData(0, sizeof(glm::mat4), glm::value_ptr(lightTrans));
		m_MVPUuniformBuffer->SetData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewProj));

		glm::vec3 camPos = camera.GetPosition();

		m_CameraUniformBuffer->SetData(camTypeOffsets[0], sizeof(glm::vec2), glm::value_ptr(gScreenSize));
		m_CameraUniformBuffer->SetData(camTypeOffsets[1], sizeof(glm::vec3), glm::value_ptr(camPos));
		//m_LightPass->SetFloat2("gScreenSize", gScreenSize);
		//m_LightPass->SetFloat3("camPos", camera.GetPosition());

		//Set Light Attribute
		/*m_LightPass->SetFloat3("light.position", Trans[LightIndex].Translation);
		m_LightPass->SetFloat3("light.color", Lights[LightIndex].color);
		m_LightPass->SetFloat("light.constant", Lights[LightIndex].constant);
		m_LightPass->SetFloat("light.linear", Lights[LightIndex].linear);
		m_LightPass->SetFloat("light.quadratic", Lights[LightIndex].quadratic);*/

		m_lightUniformBuffer->SetData(lightTypeOffsets[0], sizeof(glm::vec3), glm::value_ptr(Trans[LightIndex].Translation));
		m_lightUniformBuffer->SetData(lightTypeOffsets[1], sizeof(glm::vec3), glm::value_ptr(Lights[LightIndex].color));
		m_lightUniformBuffer->SetData(lightTypeOffsets[2], sizeof(float), &Lights[LightIndex].constant);
		m_lightUniformBuffer->SetData(lightTypeOffsets[3], sizeof(float), &Lights[LightIndex].linear);
		m_lightUniformBuffer->SetData(lightTypeOffsets[4], sizeof(float), &Lights[LightIndex].quadratic);
		SphereModel.Draw();
		//cull back
		RenderCommand::CullFrontOrBack(0);
		//close blend
		RenderCommand::Blend(0);
		RenderCommand::DepthTest(1);
		m_LightPass->Unbind();
	}

	static void LightPass(Camera& camera, TransformComponent& trans, glm::vec2& gScreenSize, int32_t LightIndex, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Ref<Framebuffer> geoPassFramebuffer, Ref<Framebuffer> LightPassFramebuffer)
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

		//m_LightPass->SetMat4("u_ViewProjection", viewProj);
		//m_LightPass->SetMat4("u_Model", Trans[LightIndex].GetTransform());
		glm::mat4 lightTrans = Trans[LightIndex].GetTransform();
		m_MVPUuniformBuffer->SetData(0, sizeof(glm::mat4), glm::value_ptr(lightTrans));
		m_MVPUuniformBuffer->SetData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(viewProj));

		//m_LightPass->SetFloat2("gScreenSize", gScreenSize);
		//m_LightPass->SetFloat3("camPos", trans.Translation);
		glm::vec3 camPos = trans.Translation;
		m_CameraUniformBuffer->SetData(camTypeOffsets[0], sizeof(glm::vec2), glm::value_ptr(gScreenSize));
		m_CameraUniformBuffer->SetData(camTypeOffsets[1], sizeof(glm::vec3), glm::value_ptr(camPos));

		//Set Light Attribute
		/*m_LightPass->SetFloat3("light.position", Trans[LightIndex].Translation);
		m_LightPass->SetFloat3("light.color", Lights[LightIndex].color);
		m_LightPass->SetFloat("light.constant", Lights[LightIndex].constant);
		m_LightPass->SetFloat("light.linear", Lights[LightIndex].linear);
		m_LightPass->SetFloat("light.quadratic", Lights[LightIndex].quadratic);*/
		m_lightUniformBuffer->SetData(lightTypeOffsets[0], sizeof(glm::vec3), glm::value_ptr(Trans[LightIndex].Translation));
		m_lightUniformBuffer->SetData(lightTypeOffsets[1], sizeof(glm::vec3), glm::value_ptr(Lights[LightIndex].color));
		m_lightUniformBuffer->SetData(lightTypeOffsets[2], sizeof(float), &Lights[LightIndex].constant);
		m_lightUniformBuffer->SetData(lightTypeOffsets[3], sizeof(float), &Lights[LightIndex].linear);
		m_lightUniformBuffer->SetData(lightTypeOffsets[4], sizeof(float), &Lights[LightIndex].quadratic);

		SphereModel.Draw();
		//cull back
		RenderCommand::CullFrontOrBack(0);
		//close blend
		RenderCommand::Blend(0);
		RenderCommand::DepthTest(1);
		m_LightPass->Unbind();
	}
}