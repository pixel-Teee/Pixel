#pragma once

#include "Pixel/ConstantBuffers.h"
#include "Pixel/Renderer/BaseRenderer.h"

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
	class DirectXRenderer : public BaseRenderer
	{
	public:
		DirectXRenderer();
		virtual ~DirectXRenderer();

		virtual uint32_t CreatePso(BufferLayout& layout) override;

		virtual uint32_t CreateDeferredPso(BufferLayout& layout) override;

		virtual void ForwardRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent>& trans,
			std::vector<StaticMeshComponent>& meshs, std::vector<LightComponent>& lights, std::vector<TransformComponent>& lightTrans, Ref<Framebuffer> pFrameBuffer, std::vector<int32_t>& entityIds) override;

		virtual void DeferredRendering(Ref<Context> pGraphicsContext, const EditorCamera& camera, std::vector<TransformComponent>& trans,
		std::vector<StaticMeshComponent>& meshs, std::vector<MaterialComponent>& materials, std::vector<LightComponent>& lights, std::vector<TransformComponent>& lightTrans,
		Ref<Framebuffer> pFrameBuffer, std::vector<int32_t>& entityIds) override;

		virtual void RenderPickerBuffer(Ref<Context> pComputeContext, Ref<Framebuffer> pFrameBuffer);

		virtual Ref<PSO> GetPso(uint32_t psoIndex) override;

		virtual int32_t GetPickerValue(uint32_t x, uint32_t y) override;
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
		Ref<PSO> m_DefaultLightShadingPso;

		Ref<Shader> m_GeometryVertexShader;
		Ref<Shader> m_GeometryPixelShader;
		//------Deferred Shading------
	};
}
 