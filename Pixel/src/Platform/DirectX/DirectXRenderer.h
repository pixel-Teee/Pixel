#pragma once

#include "Pixel/Renderer/BaseRenderer.h"
#include "Pixel/Renderer/RendererType.h"
#include "Pixel/ConstantBuffers.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorHandle.h"

namespace Pixel {
	class BufferLayout;
	//------pso related------
	class PSO;
	class RootSignature;
	class Shader;
	//------pso related------

	class Device;
	class Context;
	class GpuResource;
	class DescriptorHeap;
	class DescriptorHandle;
	class ShadowBuffer;
	class Framebuffer;
	class EditorCamera;
	class SimpleScene;
	class SamplerDesc;
	class MaterialInstance;
	class PipelineLibrary;

	struct StaticMeshContext
	{
		glm::mat4 transform;

		Ref<StaticMesh> pStaticMesh;

		Ref<SubMaterial> pSubMaterial;

		Ref<MaterialInstance> pMaterialInstance;

		int32_t entityId;

		float distance;//use for transparent

		bool operator<(const StaticMeshContext& rhs) const
		{
			return this->distance < rhs.distance;
		}
	};

	class DirectXRenderer : public BaseRenderer
	{
	public:
		DirectXRenderer();

		virtual ~DirectXRenderer();

		virtual void Initialize() override;

		virtual uint32_t CreatePso(BufferLayout& layout) override;//use for model's forward renderer

		virtual uint32_t CreateDeferredPso(BufferLayout& layout) override;//use for model's deferred geometry renderer

		virtual uint32_t CreateDeferredLightPso(BufferLayout& layout) override;//use for model's deferred lighting renderer, but just only one plane use this function

		virtual void ForwardRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent>& trans,
			std::vector<StaticMeshComponent>& meshs, std::vector<LightComponent>& lights, std::vector<TransformComponent>& lightTrans, Ref<Framebuffer> pFrameBuffer, std::vector<int32_t>& entityIds) override;

		virtual void DeferredRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent*> trans,
		std::vector<StaticMeshComponent*> meshs, std::vector<MaterialTreeComponent*> materials, std::vector<LightComponent*> lights, std::vector<TransformComponent*> lightTrans,
		Ref<Framebuffer> pFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, std::vector<int32_t>& entityIds, std::vector<Camera*> pCamera, std::vector<TransformComponent*> cameraTransformant, std::vector<int32_t> cameraEntity, StaticMeshComponent* OutLineMesh, TransformComponent* OutLineMeshTransform, Ref<Scene> scene) override;

		virtual void DeferredRenderingForSimpleScene(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent*> trans,
			std::vector<StaticMeshComponent*> meshs, std::vector<MaterialTreeComponent*> materials, std::vector<LightComponent*> lights, std::vector<TransformComponent*> lightTrans,
			Ref<Framebuffer> pFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, std::vector<int32_t>& entityIds, Ref<SimpleScene> pScene, Ref<Material> pTestMaterial);

		virtual void RenderPickerBuffer(Ref<Context> pComputeContext, Ref<Framebuffer> pFrameBuffer);

		virtual Ref<PSO> GetPso(uint32_t psoIndex, bool isTransParent) override;

		virtual int32_t GetPickerValue(uint32_t x, uint32_t y) override;

		//------called from the editor or the runtime when start------
		virtual void InitializeAndConvertHDRToCubeMap(std::string& HdrTexturePath) override;
		//------called from the editor or the runtime when start------

		//return hdr texture's cpu descriptor handle
		virtual Ref<DescriptorCpuHandle> GetHDRDescriptorHandle() override;

		virtual void DeferredRendering(Ref<Context> pGraphicsContext, Camera* pCamera, TransformComponent* pCameraTransformComponent, std::vector<TransformComponent*> trans, std::vector<StaticMeshComponent*> meshs, std::vector<MaterialTreeComponent*> materials, std::vector<LightComponent*> lights, std::vector<TransformComponent*> lightTrans, Ref<Framebuffer> pFrameBuffer, Ref<Framebuffer> pLightFrameBuffer, std::vector<int32_t>& entityIds, Ref<Scene> scene) override;

		virtual void RenderImageToBackBuffer(Ref<GpuResource> pDestResource, Ref<GpuResource> pSrcResource, Ref<Context> pContext) override;

		virtual Ref<DescriptorCpuHandle> GetShadowMapSrvHandle() override;

		virtual void DrawFrustum(Ref<Context> pGraphicsContext, const EditorCamera& camera, Camera* pCamera, TransformComponent* pCameraTransformComponent, Ref<Framebuffer> pFrameBuffer, Ref<Scene> scene) override;//editor only, draw frustum

		virtual void DrawShadowMapFrustum(Ref<Context> pGraphicsContext, const EditorCamera& camera, LightComponent* pLight, TransformComponent* pLightTransformComponent, Ref<Framebuffer> pFrameBuffer, Ref<Scene> scene) override;

		virtual void RenderBlurTexture(Ref<Context> pComputeContext, Ref<Framebuffer> pLightFrameBuffer) override;

		virtual void RenderingFinalColorBuffer(Ref<Context> pContext, Ref<Framebuffer> pSceneFrameBuffer, Ref<Framebuffer> pFinalColorBuffer) override;

		virtual void RenderPointLightVolume(Ref<Context> pGraphicsContext, const EditorCamera& camera, LightComponent* lights, TransformComponent* lightTrans, Ref<Framebuffer> pLightFrameBuffer, Ref<Scene> scene) override;

		virtual void RenderSpotLightVolume(Ref<Context> pGraphicsContext, const EditorCamera& camera, LightComponent* lights, TransformComponent* lightTrans, Ref<Framebuffer> pLightFrameBuffer) override;

		virtual void SetExposure(float exposure) override;

		virtual void SetSigmma(float sigmma) override;

		virtual Ref<DescriptorHeap> GetDescriptorHeap() override;

		virtual void ResetDescriptorHeapOffset() override;

		virtual uint32_t GetDescriptorHeapOffset() override;

		virtual void SetDescriptorHeapOffset(uint32_t offset) override;
		
		virtual Ref<DescriptorHandle> GetDescriptorHeapFirstHandle() override;

		virtual Ref<DescriptorHandle> GetNullDescriptorHandle() override;

		virtual void CreateMaterialPso(Ref<Shader> pVertexShader, Ref<Shader> pPixelShader, Ref<Material> pOriginalMaterial, BufferLayout& layout) override;

		virtual uint32_t CreateCompleteMaterialPso(uint32_t uninitializedPsoIndex, BufferLayout& layout) override;

		virtual Ref<PSO> GetUninitializedMaterialPso(uint32_t psoIndex) override;

		virtual std::vector<Ref<PSO>>& GetCompletePsos() override;

		//TODO:need to modify this to virtual function
		Ref<RootSignature> CreateRootSignature(Ref<Shader> pVertexShader, Ref<Shader> pPixelShader, std::vector<Ref<SamplerDesc>> pSampleDescs);

		Ref<PipelineLibrary> GetPipelineLibrary() override;

		virtual void DrawIntermediatePreviewNodes(Ref<Context> pGraphicsContext, Ref<Shader> pVertexShader, Ref<Shader> pPixelShader, Ref<Framebuffer> pFrameBuffer) override;

	private:

		void CreatePreviewNodePso();

		void CreateDefaultForwardRendererPso();//use for model's forward renderer

		void CreatePickerPso();

		void CreateDefaultDeferredShadingPso();//create deferred geometry shading renderer and deferred light shading renderer

		void GetShader(Ref<StaticMesh> pStaticMesh, Ref<MaterialInstance> pMaterialInstance);

		Ref<ShaderKey> GetShaderKey(Ref<StaticMesh> pStaticMesh, Ref<Material> pMaterial);
		
		//------Foward Renderer------
		Ref<PSO> m_defaultPso;
		Ref<RootSignature> m_rootSignature;
		Ref<Shader> m_forwardPs;
		Ref<Shader> m_forwardVs;
		std::vector<Ref<PSO>> m_ForwardPsoArray;//for transparent pso

		std::vector<Ref<PSO>> m_PsoArray;//for every models's pso
		GlobalConstants m_globalConstants;
		//------Foward Renderer------

		//------Picker Information------
		Ref<PSO> m_PickerPSO;
		Ref<RootSignature> m_PickerRootSignature;
		Ref<Shader> m_PickerShader;
		Ref<GpuResource> m_PickerBuffer;
		Ref<DescriptorHandle> m_UVBufferHandle;
		Ref<GpuResource> m_editorImageWidthHeightBuffer;

		Ref<DescriptorHeap> m_ComputeSrvHeap;
		Ref<DescriptorHeap> m_ComputeCbvHeap;
		Ref<DescriptorHandle> m_TextureHandle;
		Ref<DescriptorHandle> m_ImageWidthHandle;

		uint32_t m_Width;
		uint32_t m_Height;

		uint32_t m_lastWidth;
		uint32_t m_lastHeight;
		//------Picker Information------

		//------Deferred Geometry Shading------
		Ref<PSO> m_DefaultGeometryShadingPso;
		Ref<PSO> m_DefaultGeometryShadingTransParentPso;
		Ref<RootSignature> m_pDeferredShadingRootSignature;
		Ref<Shader> m_GeometryVertexShader;
		Ref<Shader> m_GeometryPixelShader;
		//------Deferred Geometry Shading------

		//------Deferred Shading Light Pass------
		Ref<PSO> m_DefaultLightShadingPso;
		Ref<RootSignature> m_pDeferredShadingLightRootSignature;
		Ref<Shader> m_LightVertexShader;
		Ref<Shader> m_LightPixelShader;

		Ref<VertexBuffer> m_pVertexBuffer;
		Ref<IndexBuffer> m_pIndexBuffer;//for plane

		LightPass m_lightPass;

		Ref<DescriptorHeap> m_DeferredShadingLightGbufferTextureHeap;//gbuffer texture handle and others texture handle
		Ref<DescriptorHandle> m_DeferredShadingLightGbufferTextureHandle;//first handle, to bind descriptor table
		std::vector<DescriptorHandle> m_DeferredShadingLightGbufferTextureHandles;//other handles

		uint32_t m_DeferredShadingLightPsoIndex;
		//------Deferred Shading Light Pass------

		//------IBL------
		void CreateConvertHDRToCubePipeline();
		void CreatePrefilterPipeline();
		void CreateLutPipeline();
		//------Equirectangular To CubeMap------
		Ref<PSO> m_HDRConvertToCubePso;
		Ref<RootSignature> m_EquirectangularToCubemapRootSignature;
		Ref<Shader> m_EquirectangularToCubemapVs;
		Ref<Shader> m_EquirectangularToCubemapFs;

		Ref<Texture> m_HDRTexture;
		Ref<DescriptorHeap> m_EquirectangularMap;
		Ref<DescriptorHandle> m_EquirectangularDescriptorHandle;

		Ref<CubeTexture> m_CubeMapTexture;//from equirectangular to cubemap texture

		Ref<Framebuffer> m_EquirectangularToCubemapFrameBuffer;
		//------Equirectangular To CubeMap------

		//------SkyBox------
		Ref<PSO> m_SkyBoxPso;
		Ref<RootSignature> m_SkyBoxRootSignature;
		Ref<Shader> m_SkyBoxVs;
		Ref<Shader> m_SkyBoxPs;
		Ref<DescriptorHeap> m_SkyBoxHeap;
		Ref<DescriptorHandle> m_SkyBoxHeapTextureHandle;
		//------SkyBox------

		//------irradiance cubemap texture------
		Ref<RootSignature> m_convolutionRootSignature;
		Ref<PSO> m_convolutionPso;
		Ref<Shader> m_convolutionVs;
		Ref<Shader> m_convolutionPs;
		Ref<CubeTexture> m_irradianceCubeTexture;//irradiance(fu zhao du)
		Ref<DescriptorHeap> m_irradianceCubeTextureHeap;
		Ref<DescriptorHandle> m_irradianceCubeTextureHandle;
		//------irradiance cubemap texture-------

		//------prefilter cubemap texture------
		Ref<PSO> m_prefilterPso;
		Ref<RootSignature> m_prefilterRootSignature;
		Ref<Shader> m_prefilterVs;
		Ref<Shader> m_prefilterPs;
		Ref<CubeTexture> m_prefilterMap;
		Ref<Framebuffer> m_prefilterFrameBuffer[5];//5 mipmaps
		//------prefilter cubemap texture------

		//------lut texture------
		Ref<PSO> m_LutPso;
		Ref<RootSignature> m_LutRootSignature;
		Ref<Texture2D> m_LutTexture;
		Ref<Shader> m_LutVs;
		Ref<Shader> m_LutPs;
		Ref<Framebuffer> m_LutFrameBuffer;
		//------lut texture------
		//------IBL------

		//------quad and cube vertex buffer and index buffer------
		Ref<VertexBuffer> m_QuadVertexBuffer;
		Ref<IndexBuffer> m_QuadIndexBuffer;
		Ref<VertexBuffer> m_CubeVertexBuffer;
		Ref<IndexBuffer> m_CubeIndexBuffer;
		//------quad and cube vertex buffer and index buffer------

		//------render image to back buffer------
		void CreateRenderImageToBackBufferPipeline();
		Ref<PSO> m_ImageToBackBufferPso;
		Ref<RootSignature> m_ImageToBackBufferRootSignature;
		Ref<Shader> m_ImageToBackBufferVs;
		Ref<Shader> m_ImageToBackBufferPs;
		Ref<DescriptorHeap> m_ImageDescriptorHeap;
		Ref<DescriptorHandle> m_ImageDescriptorHandle;
		//------render image to back buffer------

		//------shadow map------
		void CreateRenderShadowMapPipeline();
		Ref<PSO> m_RenderShadowMapPso;
		Ref<RootSignature> m_RenderShadowMapRootSignature;
		Ref<Shader> m_RenderShadowMapVs;
		Ref<Shader> m_RenderShadowMapPs;
		Ref<ShadowBuffer> m_ShadowMap;
		//------shadow map------

		//------render camera frustum------
		void CreateCameraFrustumPipeline();
		Ref<PSO> m_CameraFrustumPso;
		Ref<RootSignature> m_CameraFrustumRootSignature;
		Ref<Shader> m_CameraFrustumVs;
		Ref<Shader> m_CameraFrustumPs;
		Ref<Model> pCameraModel;
		Ref<MaterialComponent> pCameraMaterialComponent;
		//------render camera frustum------

		//------bloom------
		void CreateBlurPipeline();
		Ref<Shader> m_HorzBlurShader;
		Ref<Shader> m_VertBlurShader;
		Ref<PSO> m_HorzBlurPso;
		Ref<PSO> m_VertBlurPso;
		Ref<RootSignature> m_BlurRootSignature;
		Ref<RootSignature> m_Blur2RootSignature;
		Ref<GpuResource> m_BlurTexture;
		Ref<GpuResource> m_BlurTexture2;
		Ref<DescriptorHeap> m_BlurTextureUavSrvHeap;
		Ref<DescriptorHandle> m_BlurTextureUavHandle;//horz
		Ref<DescriptorHandle> m_BlurTexture2SrvHandle;

		Ref<DescriptorHandle> m_BlurTexture2UavHandle;//vert
		Ref<DescriptorHandle> m_BlurTextureSrvHandle;

		float m_sigmma = 2.5f;
		//------bloom------

		//------additive blending------
		void CreateAdditiveBlendingPipeline();
		Ref<PSO> m_AdditiveBlendingPso;
		Ref<RootSignature> m_AdditiveRootSignature;
		Ref<Shader> m_AdditiveBlendingVs;
		Ref<Shader> m_AdditiveBlendingPs;
		Ref<DescriptorHeap> m_AdditiveBlendingDescriptorHeap;
		Ref<DescriptorHandle> m_AdditiveBlendingDescriptorHandle;
		Ref<DescriptorHandle> m_AdditiveBlendingDescriptorHandle2;
		//------additive blending------

		//------debug point light wireframe------
		void CreatePointLightVolumePipeline();
		Ref<PSO> m_PointLightVolumePso;
		Ref<RootSignature> m_PointLightVolumeRootSignature;
		Ref<Shader> m_PointLightVolumeVs;
		Ref<Shader> m_PointLightVolumePs;
		Ref<VertexBuffer> m_PointLightVolumeVertex;
		Ref<IndexBuffer> m_PointLightVolumeIndex;
		//------debug point light wireframe------

		//------outline------
		void CreateOutlinePipeline();
		Ref<PSO> m_OutlinePso;
		Ref<RootSignature> m_OutlineRootSignature;
		Ref<Shader> m_OutlineVs;
		Ref<Shader> m_OutlinePs;
		Ref<VertexBuffer> m_OutlineVertex;
		Ref<IndexBuffer> m_OutlineIndex;
		//OutLinePass m_OutLinePass;
		//------outline------

		//------TAA------
		uint32_t m_FrameCount;
		bool m_IsFirst;
		CbufferGeometryPass m_CbufferGeometryPass;
		void CreateTAAPipeline();
		
	private:
		Ref<PSO> m_TAAPSO;
		Ref<RootSignature> m_TAARootSignature;
		Ref<Shader> m_TAAVs;
		Ref<Shader> m_TAAPs;
		Ref<DescriptorHeap> m_TAATextureDescriptorHeap;
		Ref<DescriptorHandle> m_TAAFirstTextureDescriptorHandle;
		std::vector<DescriptorHandle> m_TAAFirstTextureDescriptorHandles;

		Ref<GpuResource> m_PreviousScene;//previous scene texture
		Ref<GpuResource> m_CurrentScene;//current scene texture
		//------TAA------

		float m_exposure;

		//------for direct light------
		Ref<Model> m_pArrowModel;
		Ref<MaterialComponent> m_pArrowModelMaterial;
		//------for direct light------

		std::vector<StaticMeshContext> m_OpaqueItems;

		std::vector<StaticMeshContext> m_TransParentItems;

		//------total bind texture descriptor heap------
		Ref<DescriptorHeap> m_TotalBindTextureDescriptorHeap;
		Ref<DescriptorHandle> m_TotalBindTextureDescriptorHeapFirstHandle;
		Ref<DescriptorHandle> m_NullDescriptor;
		uint32_t m_Offset;//offset item
		//------total bind texture descriptor heap------

		//------all material's uninitialized pso, will in terms of the vertex input layout to create complete pso------
		std::vector<Ref<PSO>> m_MaterialPso;
		Ref<Shader> m_TestVertexShader;
		Ref<Shader> m_TestFragShader;
		//------all material's uninitialized pso, will in terms of the vertex input layout to create complete pso------

		//------pipeline state library------
		Ref<PipelineLibrary> m_PipelineLibrary;
		//------pipeline state library------

		//------preview node pipeline state object------
		std::vector<Ref<PSO>> m_PreviewNodePsos;
		Ref<PSO> m_PreviewNodePso;
		std::vector<Ref<Shader>> m_TempShader;
		std::vector<Ref<RootSignature>> m_TempRootSignature;
		//------preview node pipeline state object------
	};
}
 