#pragma once

#include "Pixel/Scene/Components.h"

namespace Pixel {
	class BufferLayout;
	class Device;

	class EditorCamera;
	class Framebuffer;
	class Context;
	class PSO;
	class GpuResource;
	class Scene;
	class DescriptorHeap;
	class SimpleScene;
	class ThumbNailScene;
	class MaterialTreeComponent;
	class PipelineLibrary;
	//renderer interface
	class BaseRenderer
	{
	public:
		virtual ~BaseRenderer();

		virtual void Initialize() = 0;

		virtual uint32_t CreatePso(BufferLayout& layout) = 0;

		virtual uint32_t CreateDeferredPso(BufferLayout& layout) = 0;

		virtual uint32_t CreateDeferredLightPso(BufferLayout& layout) = 0;

		virtual void CreateMaterialPso(Ref<Shader> pVertexShader, Ref<Shader> pPixelShader, Ref<Material> pOriginalMaterial, BufferLayout& layout) = 0;

		virtual uint32_t CreateCompleteMaterialPso(uint32_t uninitializedPsoIndex, BufferLayout& layout) = 0;

		virtual Ref<PSO> GetPso(uint32_t psoIndex, bool isTransParent) = 0;

		virtual Ref<PSO> GetUninitializedMaterialPso(uint32_t psoIndex) = 0;

		virtual void ForwardRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent>& trans,
			std::vector<StaticMeshComponent>& meshs, std::vector<LightComponent>& lights, std::vector<TransformComponent>& lightTrans, Ref<Framebuffer> pFrameBuffer, std::vector<int32_t>& entityIds) = 0;

		virtual void DeferredRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent*> trans,
			std::vector<StaticMeshComponent*> meshs, std::vector<MaterialTreeComponent*> materials, std::vector<LightComponent*> lights, std::vector<TransformComponent*> lightTrans,
			Ref<Framebuffer> pFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, std::vector<int32_t>& entityIds, std::vector<Camera*> pCamera, std::vector<TransformComponent*> cameraTransformant, std::vector<int32_t> cameraEntity, StaticMeshComponent* OutLineMesh, TransformComponent* OutLineMeshTransform, Ref<Scene> scene) = 0;

		virtual void DeferredRendering(Ref<Context> pGraphicsContext, Camera* pCamera, TransformComponent* pCameraTransformComponent, std::vector<TransformComponent*> trans,
			std::vector<StaticMeshComponent*> meshs, std::vector<MaterialTreeComponent*> materials, std::vector<LightComponent*> lights, std::vector<TransformComponent*> lightTrans,
			Ref<Framebuffer> pFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, std::vector<int32_t>& entityIds, Ref<Scene> scene) = 0;

		virtual void DeferredRenderingForSimpleScene(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent*> trans,
			std::vector<StaticMeshComponent*> meshs, std::vector<MaterialTreeComponent*> materials, std::vector<LightComponent*> lights, std::vector<TransformComponent*> lightTrans,
			Ref<Framebuffer> pFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, std::vector<int32_t>& entityIds, Ref<SimpleScene> pScene, Ref<Material> pTestMaterial) = 0;

		virtual void DeferredRenderingForThumbNailScene(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent*> trans,
			std::vector<StaticMeshComponent*> meshs, std::vector<MaterialTreeComponent*> materials, std::vector<LightComponent*> lights, std::vector<TransformComponent*> lightTrans,
			Ref<Framebuffer> pFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, std::vector<int32_t>& entityIds, Ref<ThumbNailScene> pScene, Ref<Material> pTestMaterial) = 0;

		virtual void RenderPickerBuffer(Ref<Context> pComputeContext, Ref<Framebuffer> pFrameBuffer) = 0;

		virtual void DrawFrustum(Ref<Context> pGraphicsContext, const EditorCamera& camera, Camera* pCamera, TransformComponent* pCameraTransformComponent, Ref<Framebuffer> pFrameBuffer, Ref<Scene> scene) = 0;

		virtual void DrawShadowMapFrustum(Ref<Context> pGraphicsContext, const EditorCamera& camera, LightComponent* pLight, TransformComponent* pLightTransformComponent, Ref<Framebuffer> pFrameBuffer, Ref<Scene> scene) = 0;

		virtual void RenderPointLightVolume(Ref<Context> pGraphicsContext, const EditorCamera& camera, LightComponent* lights, TransformComponent* lightTrans, Ref<Framebuffer> pLightFrameBuffer, Ref<Scene> scene) = 0;

		virtual void RenderSpotLightVolume(Ref<Context> pGraphicsContext, const EditorCamera& camera, LightComponent* lights, TransformComponent* lightTrans, Ref<Framebuffer> pLightFrameBuffer) = 0;

		virtual void RenderImageToBackBuffer(Ref<GpuResource> pDestResource, Ref<GpuResource> pSrcResource, Ref<Context> pContext) = 0;

		virtual int32_t GetPickerValue(uint32_t x, uint32_t y) = 0;

		virtual void InitializeAndConvertHDRToCubeMap(std::string& HdrTexturePath) = 0;

		virtual Ref<DescriptorCpuHandle> GetHDRDescriptorHandle() = 0;

		virtual Ref<DescriptorCpuHandle> GetShadowMapSrvHandle() = 0;

		virtual void RenderBlurTexture(Ref<Context> pComputeContext, Ref<Framebuffer> pLightFrameBuffer) = 0;

		virtual void RenderingFinalColorBuffer(Ref<Context> pContext, Ref<Framebuffer> pSceneFrameBuffer, Ref<Framebuffer> pFinalColorBuffer) = 0;

		//copy image from final color buffer to texture
		virtual void GenerateThumbNail(Ref<Framebuffer> pFinalColorBuffer, Ref<Texture2D>& pTexture, const std::string& physicalPath) = 0;

		virtual void DrawIntermediatePreviewNodes(Ref<Context> pGraphicsContext, Ref<Shader> pVertexShaderFunction, Ref<Shader> pPixelShaderFunction, Ref<Framebuffer> pFrameBuffer, Ref<Material> pMaterial) = 0;

		virtual void SetExposure(float exposure) = 0;

		virtual void SetSigmma(float sigmma) = 0;

		virtual Ref<DescriptorHeap> GetDescriptorHeap() = 0;

		virtual void ResetDescriptorHeapOffset() = 0;

		virtual uint32_t GetDescriptorHeapOffset() = 0;

		virtual void SetDescriptorHeapOffset(uint32_t offset) = 0;

		virtual Ref<DescriptorHandle> GetDescriptorHeapFirstHandle() = 0;

		virtual Ref<DescriptorHandle> GetNullDescriptorHandle() = 0;

		virtual std::vector<Ref<PSO>>& GetCompletePsos() = 0;

		virtual Ref<PipelineLibrary> GetPipelineLibrary() = 0;

		static Ref<BaseRenderer> Create();
	};
}