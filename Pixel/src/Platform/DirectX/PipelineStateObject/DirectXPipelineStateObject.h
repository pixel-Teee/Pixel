#pragma once

#include "Platform/DirectX/Context/DirectXContext.h"
#include "Pixel/Renderer/PipelineStateObject/PipelineStateObject.h"

namespace Pixel {
	class RootSignature;

	class DirectXPSO : public PSO
	{
	public:
		DirectXPSO();
		DirectXPSO(const wchar_t* Name) : m_Name(Name) {}

		//clear hash map
		static void DestroyAll();

		virtual void SetRootSignature(Ref<RootSignature> pRootSignature) = 0;
		virtual void SetBlendState(Ref<BlenderState> pBlendState) = 0;
		virtual void SetRasterizerState(Ref<RasterState> pRasterState) = 0;
		virtual void SetDepthState(Ref<DepthState> pDepthState) = 0;
		virtual void SetPrimitiveTopologyType(PiplinePrimitiveTopology TopologyType) = 0;
		virtual bool IsMatchPso(BufferLayout layout, Ref<RootSignature> pRootSignature) = 0;
		virtual void SetInputLayout(BufferLayout& vertexLayout) = 0;

		virtual Ref<RootSignature> GetRootSignature() const override;

		//get pipeline state
		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineStateObject() const { return m_pPSO; }

	protected:
		const wchar_t* m_Name;

		Ref<RootSignature> m_pRootSignature;

		//pipeline state object
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPSO;

		size_t m_HashCode;
	};

	class BlenderState;
	class DepthState;

	class GraphicsPSO : public DirectXPSO
	{
	public:

		//start with empty state
		GraphicsPSO(const wchar_t* Name = L"Unnamed Graphics PSO");

		GraphicsPSO(const GraphicsPSO& pso);

		virtual void SetBlendState(Ref<BlenderState> pBlendState) override;

		virtual void SetRasterizerState(Ref<RasterState> pRasterState) override;

		virtual void SetDepthState(Ref<DepthState> pDepthState) override;

		void SetBlendState(const D3D12_BLEND_DESC& BlendDesc);

		void SetRasterizerState(const D3D12_RASTERIZER_DESC& RasterizerDesc);

		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& DepthStencilDesc);

		void SetSampleMask(uint32_t SampleMask);

		virtual void SetPrimitiveTopologyType(PiplinePrimitiveTopology TopologyType);

		virtual void SetDepthTargetFormat(ImageFormat DSVFormat, uint32_t MsaaCount = 1, uint32_t MsaaQuality = 0) override;

		//render target view, depth stencil view
		virtual void SetRenderTargetFormat(ImageFormat RTVFormat, ImageFormat DSVFormat, uint32_t MsaaCount = 1, uint32_t MsaaQuality = 0) override;

		//multiple render targets
		virtual void SetRenderTargetFormats(uint32_t NumRTVs, const ImageFormat* RTVFormats, ImageFormat DSVFormat, uint32_t MsaaCount = 1,
			uint32_t MsaaQuality = 0) override;

		//input layout
		void SetInputLayout(uint32_t NumElements, const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs);

		virtual void SetInputLayout(BufferLayout& vertexLayout);

		void SetVertexShader(const void* Binary, size_t Size);
		void SetPixelShader(const void* Binary, size_t Size);

		virtual void SetRootSignature(Ref<RootSignature> pRootSignature) override;

		//perform validation and compute a hash value for fast state block comparisons
		virtual void Finalize() override;

		//------Compute PSO's operation------
		virtual void SetComputeShader(const void* Binary, size_t Size) override;
		//------Compute PSO's operation------

		virtual bool IsMatchPso(BufferLayout layout, Ref<RootSignature> pRootSignature) override;
	private:

		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_PSODesc;

		Ref<D3D12_INPUT_ELEMENT_DESC> m_InputLayouts;

		BufferLayout m_Layout;
	};

	class ComputePSO : public DirectXPSO
	{
	public:
		ComputePSO(const wchar_t* Name = L"Unnamed Compute PSO");

		virtual void SetComputeShader(const void* Binary, size_t Size) override;

		virtual void SetRootSignature(Ref<RootSignature> pRootSignature) override;
		virtual void Finalize() override;

		//------garbage------
		virtual void SetBlendState(Ref<BlenderState> pBlendState) override;
		virtual void SetRasterizerState(Ref<RasterState> pRasterState) override;
		virtual void SetDepthState(Ref<DepthState> pDepthState) override;
		virtual void SetPrimitiveTopologyType(PiplinePrimitiveTopology TopologyType) override;
		virtual void SetDepthTargetFormat(ImageFormat DSVFormat, uint32_t MsaaCount = 1, uint32_t MsaaQuality = 0) override;
		virtual void SetRenderTargetFormat(ImageFormat RTVFormat, ImageFormat DSVFormat, uint32_t MsaaCount = 1, uint32_t MsaaQuality = 0) override;
		virtual void SetRenderTargetFormats(uint32_t NumRTVs, const ImageFormat* RTVFormats, ImageFormat DSVFormat, uint32_t MsaaCount = 1, uint32_t MsaaQuality = 0) override;
		virtual void SetVertexShader(const void* Binary, size_t Size) override;
		virtual void SetPixelShader(const void* Binary, size_t Size) override;
		virtual void SetInputLayout(BufferLayout& vertexLayout) override;
		//------garbage------

		virtual bool IsMatchPso(BufferLayout layout, Ref<RootSignature> pRootSignature) override;
	private:
		D3D12_COMPUTE_PIPELINE_STATE_DESC m_PSODesc;	
	};
}