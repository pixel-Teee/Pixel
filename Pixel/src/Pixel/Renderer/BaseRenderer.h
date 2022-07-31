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

	//renderer interface
	class BaseRenderer
	{
	public:
		virtual ~BaseRenderer();

		virtual void Initialize() = 0;

		virtual uint32_t CreatePso(BufferLayout& layout) = 0;

		virtual uint32_t CreateDeferredPso(BufferLayout& layout) = 0;

		virtual uint32_t CreateDeferredLightPso(BufferLayout& layout) = 0;

		virtual Ref<PSO> GetPso(uint32_t psoIndex) = 0;

		virtual void ForwardRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent>& trans,
			std::vector<StaticMeshComponent>& meshs, std::vector<LightComponent>& lights, std::vector<TransformComponent>& lightTrans, Ref<Framebuffer> pFrameBuffer, std::vector<int32_t>& entityIds) = 0;

		virtual void DeferredRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent*> trans,
			std::vector<StaticMeshComponent*> meshs, std::vector<MaterialComponent*> materials, std::vector<LightComponent*> lights, std::vector<TransformComponent*> lightTrans,
			Ref<Framebuffer> pFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, std::vector<int32_t>& entityIds, std::vector<Camera*> pCamera, std::vector<TransformComponent*> cameraTransformant, std::vector<int32_t> cameraEntity, StaticMeshComponent* OutLineMesh, TransformComponent* OutLineMeshTransform, Ref<Scene> scene) = 0;

		virtual void DeferredRendering(Ref<Context> pGraphicsContext, Camera* pCamera, TransformComponent* pCameraTransformComponent, std::vector<TransformComponent*> trans,
			std::vector<StaticMeshComponent*> meshs, std::vector<MaterialComponent*> materials, std::vector<LightComponent*> lights, std::vector<TransformComponent*> lightTrans,
			Ref<Framebuffer> pFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, std::vector<int32_t>& entityIds, Ref<Scene> scene) = 0;

		virtual void RenderPickerBuffer(Ref<Context> pComputeContext, Ref<Framebuffer> pFrameBuffer) = 0;

		virtual void DrawFrustum(Ref<Context> pGraphicsContext, const EditorCamera& camera, Camera* pCamera, TransformComponent* pCameraTransformComponent, Ref<Framebuffer> pFrameBuffer, Ref<Scene> scene) = 0;

		virtual void RenderPointLightVolume(Ref<Context> pGraphicsContext, const EditorCamera& camera, LightComponent* lights, TransformComponent* lightTrans, Ref<Framebuffer> pLightFrameBuffer, Ref<Scene> scene) = 0;

		virtual void RenderImageToBackBuffer(Ref<GpuResource> pDestResource, Ref<GpuResource> pSrcResource, Ref<Context> pContext) = 0;

		virtual int32_t GetPickerValue(uint32_t x, uint32_t y) = 0;

		virtual void InitializeAndConvertHDRToCubeMap(std::string& HdrTexturePath) = 0;

		virtual Ref<DescriptorCpuHandle> GetHDRDescriptorHandle() = 0;

		virtual Ref<DescriptorCpuHandle> GetShadowMapSrvHandle() = 0;

		virtual void RenderBlurTexture(Ref<Context> pComputeContext, Ref<Framebuffer> pLightFrameBuffer) = 0;

		virtual void RenderingFinalColorBuffer(Ref<Context> pContext, Ref<Framebuffer> pSceneFrameBuffer, Ref<Framebuffer> pFinalColorBuffer) = 0;

		virtual void SetExposure(float exposure) = 0;

		static Ref<BaseRenderer> Create();
	};
}