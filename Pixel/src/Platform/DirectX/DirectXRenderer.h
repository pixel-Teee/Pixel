#pragma once

#include "Pixel/ConstantBuffers.h"
#include "Pixel/Renderer/BaseRenderer.h"
#include "Pixel/Renderer/RendererType.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorHandle.h"

namespace Pixel {
	class BufferLayout;
	class PSO;
	class Device;
	class RootSignature;
	class Shader;
	class Context;
	class GpuResource;

	class EditorCamera;
	class Framebuffer;
	class DescriptorHeap;
	class DescriptorHandle;
	class ShadowBuffer;
	class DirectXRenderer : public BaseRenderer
	{
	public:
		DirectXRenderer();
		virtual ~DirectXRenderer();

		virtual uint32_t CreatePso(BufferLayout& layout) override;

		virtual uint32_t CreateDeferredPso(BufferLayout& layout) override;

		virtual uint32_t CreateDeferredLightPso(BufferLayout& layout) override;

		virtual void ForwardRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent>& trans,
			std::vector<StaticMeshComponent>& meshs, std::vector<LightComponent>& lights, std::vector<TransformComponent>& lightTrans, Ref<Framebuffer> pFrameBuffer, std::vector<int32_t>& entityIds) override;

		virtual void DeferredRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent*> trans,
		std::vector<StaticMeshComponent*> meshs, std::vector<MaterialComponent*> materials, std::vector<LightComponent*> lights, std::vector<TransformComponent*> lightTrans,
		Ref<Framebuffer> pFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, std::vector<int32_t>& entityIds) override;

		virtual void RenderPickerBuffer(Ref<Context> pComputeContext, Ref<Framebuffer> pFrameBuffer);

		virtual Ref<PSO> GetPso(uint32_t psoIndex) override;

		virtual int32_t GetPickerValue(uint32_t x, uint32_t y) override;

		virtual void InitializeAndConvertHDRToCubeMap(std::string& HdrTexturePath) override;

		//return hdr texture's cpu descriptor handle
		virtual Ref<DescriptorCpuHandle> GetHDRDescriptorHandle() override;

		virtual void DeferredRendering(Ref<Context> pGraphicsContext, Camera* pCamera, TransformComponent* pCameraTransformComponent, std::vector<TransformComponent*> trans, std::vector<StaticMeshComponent*> meshs, std::vector<MaterialComponent*> materials, std::vector<LightComponent*> lights, std::vector<TransformComponent*> lightTrans, Ref<Framebuffer> pFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, std::vector<int32_t>& entityIds) override;

		virtual void RenderImageToBackBuffer(Ref<GpuResource> pDestResource, Ref<GpuResource> pSrcResource, Ref<Context> pContext) override;

		virtual Ref<DescriptorCpuHandle> GetShadowMapSrvHandle() override;

		virtual void DrawFrustum(Ref<Context> pGraphicsContext, const EditorCamera& camera, Camera* pCamera, TransformComponent* pCameraTransformComponent, Ref<Framebuffer> pFrameBuffer) override;//editor only, draw frustum
	private:
		
		Ref<RootSignature> m_rootSignature;

		std::vector<Ref<PSO>> m_PsoArray;

		Ref<PSO> m_defaultPso;
		Ref<Shader> m_forwardPs;
		Ref<Shader> m_forwardVs;

		GlobalConstants m_globalConstants;

		//------Picker Information------
		Ref<DescriptorHeap> m_ComputeSrvHeap;
		Ref<DescriptorHeap> m_ComputeCbvHeap;

		Ref<GpuResource> m_PickerBuffer;
		//Ref<GpuResource> m_UVDebugBuffer;
		Ref<PSO> m_PickerPSO;
		Ref<RootSignature> m_PickerRootSignature;
		Ref<Shader> m_PickerShader;
		Ref<DescriptorHandle> m_TextureHandle;
		uint32_t m_Width;
		uint32_t m_Height;

		uint32_t m_lastWidth;
		uint32_t m_lastHeight;
		
		Ref<DescriptorHandle> m_UVBufferHandle;

		Ref<GpuResource> m_editorImageWidthHeightBuffer;
		Ref<DescriptorHandle> m_ImageWidthHandle;
		//------Picker Information------

		//------Deferred Shading------
		void CreateDefaultDeferredShadingPso();
		Ref<RootSignature> m_pDeferredShadingRootSignature;
		Ref<PSO> m_DefaultGeometryShadingPso;
		
		Ref<Shader> m_GeometryVertexShader;
		Ref<Shader> m_GeometryPixelShader;
		//------Deferred Shading------

		//------Deferred Shading Light Pass------
		Ref<PSO> m_DefaultLightShadingPso;
		Ref<RootSignature> m_pDeferredShadingLightRootSignature;
		Ref<Shader> m_LightVertexShader;
		Ref<Shader> m_LightPixelShader;
		uint32_t m_DeferredShadingLightPsoIndex;

		Ref<VertexBuffer> m_pVertexBuffer;
		Ref<IndexBuffer> m_pIndexBuffer;

		LightPass m_lightPass;

		Ref<DescriptorHeap> m_DeferredShadingLightGbufferTextureHeap;
		Ref<DescriptorHandle> m_DeferredShadingLightGbufferTextureHandle;
		std::vector<DescriptorHandle> m_DeferredShadingLightGbufferTextureHandles;
		//------Deferred Shading Light Pass------

		//------IBL------
		Ref<Texture> m_HDRTexture;
		Ref<PSO> m_HDRConvertToCubePso;
		Ref<Shader> m_EquirectangularToCubemapVs;
		Ref<Shader> m_EquirectangularToCubemapFs;
		Ref<RootSignature> m_EquirectangularToCubemapRootSignature;
		Ref<DescriptorHeap> m_EquirectangularMap;
		Ref<DescriptorHandle> m_EquirectangularDescriptorHandle;
		Ref<Framebuffer> m_EquirectangularToCubemapFrameBuffer;
		Ref<CubeTexture> m_CubeMapTexture;

		Ref<RootSignature> m_SkyBoxRootSignature;
		Ref<PSO> m_SkyBoxPso;
		Ref<Shader> m_SkyBoxVs;
		Ref<Shader> m_SkyBoxPs;
		Ref<DescriptorHeap> m_SkyBoxHeap;
		Ref<DescriptorHandle> m_SkyBoxHeapTextureHandle;
		void CreateConvertHDRToCubePipeline();
		void CreatePrefilterPipeline();
		void CreateLutPipeline();
		Ref<RootSignature> m_convolutionRootSignature;
		Ref<PSO> m_convolutionPso;
		Ref<Shader> m_convolutionVs;
		Ref<Shader> m_convolutionPs;
		Ref<CubeTexture> m_irradianceCubeTexture;//fu zhao du
		Ref<DescriptorHeap> m_irradianceCubeTextureHeap;
		Ref<DescriptorHandle> m_irradianceCubeTextureHandle;

		Ref<CubeTexture> m_prefilterMap;
		Ref<PSO> m_prefilterPso;
		Ref<Shader> m_prefilterVs;
		Ref<Shader> m_prefilterPs;
		Ref<RootSignature> m_prefilterRootSignature;
		Ref<Framebuffer> m_prefilterFrameBuffer[5];

		Ref<Texture2D> m_LutTexture;
		Ref<PSO> m_LutPso;
		Ref<Shader> m_LutVs;
		Ref<Shader> m_LutPs;
		Ref<RootSignature> m_LutRootSignature;
		Ref<Framebuffer> m_LutFrameBuffer;
		//------IBL------

		Ref<VertexBuffer> m_QuadVertexBuffer;
		Ref<IndexBuffer> m_QuadIndexBuffer;

		Ref<VertexBuffer> m_CubeVertexBuffer;
		Ref<IndexBuffer> m_CubeIndexBuffer;

		//------render image to back buffer------
		Ref<PSO> m_ImageToBackBufferPso;
		Ref<RootSignature> m_ImageToBackBufferRootSignature;
		Ref<Shader> m_ImageToBackBufferVs;
		Ref<Shader> m_ImageToBackBufferPs;
		Ref<DescriptorHeap> m_ImageDescriptorHeap;
		Ref<DescriptorHandle> m_ImageDescriptorHandle;
		void CreateRenderImageToBackBufferPipeline();
		//------render image to back buffer------

		//------render shadow map------
		Ref<PSO> m_RenderShadowMapPso;
		Ref<RootSignature> m_RenderShadowMapRootSignature;
		Ref<Shader> m_RenderShadowMapVs;
		Ref<Shader> m_RenderShadowMapPs;
		Ref<ShadowBuffer> m_ShadowMap;
		void CreateRenderShadowMapPipeline();
		//------render shadow map------

		//------render camera frustum------
		Ref<PSO> m_CameraFrustumPso;
		Ref<RootSignature> m_CameraFrustumRootSignature;
		Ref<Shader> m_CameraFrustumVs;
		Ref<Shader> m_CameraFrustumPs;
		void CreateCameraFrustumPipeline();
		//------render camera frustum------
	};
}
 