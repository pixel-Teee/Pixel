#pragma once

#include "DirectXContext.h"

namespace Pixel {
	class RootSignature;

	class PSO
	{
	public:
		PSO(const wchar_t* Name) : m_Name(Name) {}

		//clear hash map
		static void DestroyAll();

		void SetRootSignature(Ref<RootSignature> pRootSignature);

		Ref<RootSignature> GetRootSignature() const;

		//get pipeline state
		Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineStateObject() const { return m_pPSO; }

	protected:
		const wchar_t* m_Name;

		Ref<RootSignature> m_pRootSignature;

		//pipeline state object
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pPSO;
	};

	class GraphicsPSO : public PSO
	{
	public:

		//start with empty state
		GraphicsPSO(const wchar_t* Name = L"Unnamed Graphics PSO");

		void SetBlendState(const D3D12_BLEND_DESC& BlendDesc);

		void SetRasterizerState(const D3D12_RASTERIZER_DESC& RasterizerDesc);

		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& DepthStencilDesc);

		void SetSampleMask(uint32_t SampleMask);

		void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType);

		void SetDepthTargetFormat(DXGI_FORMAT DSVFormat, uint32_t MsaaCount = 1, uint32_t MsaaQuality = 0);

		//render target view, depth stencil view
		void SetRenderTargetFormat(DXGI_FORMAT RTVFormat, DXGI_FORMAT DSVFormat, uint32_t MsaaCount = 1, uint32_t MsaaQuality = 0);

		//multiple render targets
		void SetRenderTargetFormats(uint32_t NumRTVs, const DXGI_FORMAT* RTVFormats, DXGI_FORMAT DSVFormat, uint32_t MsaaCount = 1,
			uint32_t MsaaQuality = 0);

		//input layout
		void SetInputLayout(uint32_t NumElements, const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs);

		void SetVertexShader(const void* Binary, size_t Size);
		void SetPixelShader(const void* Binary, size_t Size);

		//perform validation and compute a hash value for fast state block comparisons
		void Finalize();
	private:

		D3D12_GRAPHICS_PIPELINE_STATE_DESC m_PSODesc;

		Ref<D3D12_INPUT_ELEMENT_DESC> m_InputLayouts;
	};
}