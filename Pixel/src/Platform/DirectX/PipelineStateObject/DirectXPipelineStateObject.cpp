#include "pxpch.h"

#include "DirectXPipelineStateObject.h"
#include "DirectXRootSignature.h"
#include "Platform/DirectX/DirectXDevice.h"

#include "Platform/DirectX/TypeUtils.h"
#include "Platform/DirectX/State/DirectXBlenderState.h"
#include "Platform/DirectX/State/DirectXRasterState.h"
#include "Platform/DirectX/State/DirectXDepthState.h"

namespace Pixel {

	//graphics pipeline state object hash map
	static std::map<size_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> s_GraphicsPSOHashMap;

	//compute pipeline state object hash map
	static std::map<size_t, Microsoft::WRL::ComPtr<ID3D12PipelineState>> s_ComputePSOHashMap;

	DirectXPSO::DirectXPSO()
	{

	}

	void DirectXPSO::DestroyAll()
	{
		s_GraphicsPSOHashMap.clear();
		s_ComputePSOHashMap.clear();
	}

	void DirectXPSO::SetRootSignature(Ref<RootSignature> pRootSignature)
	{
		m_pRootSignature = pRootSignature;
	}

	Ref<RootSignature> DirectXPSO::GetRootSignature() const
	{
		return m_pRootSignature;
	}

	//------Graphics PSO------
	GraphicsPSO::GraphicsPSO(const wchar_t* Name)
	:DirectXPSO(Name)
	{
		//m_pPSO = {};
		ZeroMemory(&m_PSODesc, sizeof(m_PSODesc));
		m_PSODesc.SampleMask = UINT_MAX;
		m_PSODesc.NodeMask = 1;
		m_PSODesc.SampleDesc.Count = 1;
		m_PSODesc.InputLayout.NumElements = 0;
		//m_Name = Name;
	}

	GraphicsPSO::GraphicsPSO(const GraphicsPSO& pso)
	{
		m_PSODesc = pso.m_PSODesc;
		m_InputLayouts = pso.m_InputLayouts;
		m_pRootSignature = pso.m_pRootSignature;
		m_Name = pso.m_Name;
	}

	void GraphicsPSO::SetBlendState(const D3D12_BLEND_DESC& BlendDesc)
	{
		m_PSODesc.BlendState = BlendDesc;
	}

	void GraphicsPSO::SetBlendState(Ref<BlenderState> pBlendState)
	{
		Ref<DirectXBlenderState> dxBlendState = std::static_pointer_cast<DirectXBlenderState>(pBlendState);

		m_PSODesc.BlendState = dxBlendState->m_CurrentBlend;
	}

	void GraphicsPSO::SetRasterizerState(const D3D12_RASTERIZER_DESC& RasterizerDesc)
	{
		m_PSODesc.RasterizerState = RasterizerDesc;
	}

	void GraphicsPSO::SetRasterizerState(Ref<RasterState> pRasterState)
	{
		Ref<DirectXRasterState> dxRasterState = std::static_pointer_cast<DirectXRasterState>(pRasterState);

		m_PSODesc.RasterizerState = dxRasterState->m_RasterizerDefault;
	}

	void GraphicsPSO::SetDepthState(Ref<DepthState> pDepthState)
	{
		Ref<DirectXDepthState> dxDepthState = std::static_pointer_cast<DirectXDepthState>(pDepthState);

		m_PSODesc.DepthStencilState = dxDepthState->m_DepthStencilDesc;
	}

	void GraphicsPSO::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& DepthStencilDesc)
	{
		m_PSODesc.DepthStencilState = DepthStencilDesc;
	}

	void GraphicsPSO::SetSampleMask(uint32_t SampleMask)
	{
		m_PSODesc.SampleMask = SampleMask;
	}

	//void GraphicsPSO::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE TopologyType)
	//{
	//	PX_CORE_ASSERT(TopologyType != D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED, "can't draw with undefined topology");
	//	m_PSODesc.PrimitiveTopologyType = TopologyType;
	//}

	void GraphicsPSO::SetPrimitiveTopologyType(PiplinePrimitiveTopology TopologyType)
	{
		m_PSODesc.PrimitiveTopologyType = PiplinePrimitiveTopologyToDirectXPrimitiveTopology(TopologyType);
	}

	void GraphicsPSO::SetDepthTargetFormat(ImageFormat DSVFormat, uint32_t MsaaCount /*= 1*/, uint32_t MsaaQuality /*= 0*/)
	{
		SetRenderTargetFormats(0, nullptr, DSVFormat, MsaaCount, MsaaQuality);
	}

	void GraphicsPSO::SetRenderTargetFormat(ImageFormat RTVFormat, ImageFormat DSVFormat, uint32_t MsaaCount /*= 1*/, uint32_t MsaaQuality /*= 0*/)
	{
		SetRenderTargetFormats(1, &RTVFormat, DSVFormat, MsaaCount, MsaaQuality);
	}

	void GraphicsPSO::SetRenderTargetFormats(uint32_t NumRTVs, const ImageFormat* RTVFormats, ImageFormat DSVFormat, uint32_t MsaaCount /*= 1*/, uint32_t MsaaQuality /*= 0*/)
	{
		PX_CORE_ASSERT(NumRTVs == 0 || RTVFormats != nullptr, "null format array conflicts with non-zeron length");

		for (uint32_t i = 0; i < NumRTVs; ++i) {
			PX_CORE_ASSERT(RTVFormats[i] != ImageFormat::PX_FORMAT_UNKNOWN, "Unknown Formats");

			m_PSODesc.RTVFormats[i] = ImageFormatToDirectXImageFormat(RTVFormats[i]);
		}
		m_PSODesc.NumRenderTargets = 8;
		//left render targets is null
		for (uint32_t i = NumRTVs; i < m_PSODesc.NumRenderTargets; ++i)
			m_PSODesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;

		m_PSODesc.NumRenderTargets = NumRTVs;
		m_PSODesc.DSVFormat = ImageFormatToDirectXImageFormat(DSVFormat);
		m_PSODesc.SampleDesc.Count = MsaaCount;
		m_PSODesc.SampleDesc.Quality = MsaaQuality;
	}

	void GraphicsPSO::SetInputLayout(uint32_t NumElements, const D3D12_INPUT_ELEMENT_DESC* pInputElementDescs)
	{
		m_PSODesc.InputLayout.NumElements = NumElements;

		if (NumElements > 0)
		{
			//allocate space
			D3D12_INPUT_ELEMENT_DESC* NewElements = (D3D12_INPUT_ELEMENT_DESC*)malloc(sizeof(D3D12_INPUT_ELEMENT_DESC) * NumElements);
			memcpy(NewElements, pInputElementDescs, NumElements * sizeof(D3D12_INPUT_ELEMENT_DESC));
			m_InputLayouts.reset(static_cast<D3D12_INPUT_ELEMENT_DESC*>(NewElements));
		}
		else
			m_InputLayouts = nullptr;
	}

	void GraphicsPSO::SetVertexShader(const void* Binary, size_t Size)
	{
		//first:const_cast
		//second:reinterpret_cast
		m_PSODesc.VS = {
			reinterpret_cast<BYTE*>(const_cast<void*>(Binary)),
			Size
		};
	}

	void GraphicsPSO::SetPixelShader(const void* Binary, size_t Size)
	{
		m_PSODesc.PS = {
			reinterpret_cast<BYTE*>(const_cast<void*>(Binary)),
			Size
		};
	}

	void GraphicsPSO::Finalize()
	{
		m_PSODesc.pRootSignature = std::static_pointer_cast<DirectXRootSignature>(m_pRootSignature)->GetNativeSignature();
		PX_CORE_ASSERT(std::static_pointer_cast<DirectXRootSignature>(m_pRootSignature)->GetNativeSignature() != nullptr, "root signature is nullptr!");
	
		m_PSODesc.InputLayout.pInputElementDescs = nullptr;
		size_t HashCode = Utility::HashState((const uint32_t*)&m_PSODesc);
		//hash the input layout elements
		HashCode = Utility::HashState((const uint32_t*)m_InputLayouts.get(), m_PSODesc.InputLayout.NumElements, HashCode);
		m_PSODesc.InputLayout.pInputElementDescs = m_InputLayouts.get();

		ID3D12PipelineState** PSORef = nullptr;

		bool firstCompile = false;
		{
			std::mutex s_HashMapMutex;
			std::lock_guard<std::mutex> CS(s_HashMapMutex);

			auto iter = s_GraphicsPSOHashMap.find(HashCode);

			if (iter == s_GraphicsPSOHashMap.end())
			{
				firstCompile = true;
				PSORef = s_GraphicsPSOHashMap[HashCode].GetAddressOf();
			}
			else
			{
				PSORef = iter->second.GetAddressOf();
			}
		}

		if (firstCompile)
		{
			PX_CORE_ASSERT(m_PSODesc.DepthStencilState.DepthEnable != (m_PSODesc.DSVFormat == DXGI_FORMAT_UNKNOWN),
				"unknown depth stencil view format");

			PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateGraphicsPipelineState(&m_PSODesc, IID_PPV_ARGS(m_pPSO.GetAddressOf())) >= 0,
				"Create Pipeline State Object Error!");

			s_GraphicsPSOHashMap[HashCode] = m_pPSO;
			m_pPSO->SetName(m_Name);
		}
		else
		{
			while (*PSORef == nullptr)
				std::this_thread::yield();
			m_pPSO = *PSORef;
		}
	}

	void GraphicsPSO::SetComputeShader(const void* Binary, size_t Size)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void GraphicsPSO::SetRootSignature(Ref<RootSignature> pRootSignature)
	{
		//throw std::logic_error("The method or operation is not implemented.");
		m_pRootSignature = pRootSignature;
	}

	//------Graphics PSO------

	ComputePSO::ComputePSO(const wchar_t* Name /*= L"Unnamed Compute PSO"*/)
		:DirectXPSO(Name)
	{
		ZeroMemory(&m_PSODesc, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
		//m_PSODesc.NodeMask = UINT_MAX;
	}

	void ComputePSO::SetComputeShader(const void* Binary, size_t Size)
	{
		m_PSODesc.CS = {
			reinterpret_cast<BYTE*>(const_cast<void*>(Binary)),
			Size
		};
	}

	void ComputePSO::SetRootSignature(Ref<RootSignature> pRootSignature)
	{
		m_pRootSignature = pRootSignature;
	}

	void ComputePSO::SetBlendState(Ref<BlenderState> pBlendState)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void ComputePSO::SetRasterizerState(Ref<RasterState> pRasterState)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void ComputePSO::SetDepthState(Ref<DepthState> pDepthState)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void ComputePSO::SetPrimitiveTopologyType(PiplinePrimitiveTopology TopologyType)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void ComputePSO::Finalize()
	{
		m_PSODesc.pRootSignature = std::static_pointer_cast<DirectXRootSignature>(m_pRootSignature)->GetNativeSignature();
		PX_CORE_ASSERT(std::static_pointer_cast<DirectXRootSignature>(m_pRootSignature)->GetNativeSignature() != nullptr, "root signature is nullptr!");

		size_t HashCode = Utility::HashState((const uint32_t*)&m_PSODesc);

		ID3D12PipelineState** PSORef = nullptr;

		bool firstCompile = false;
		{
			std::mutex s_HashMapMutex;
			std::lock_guard<std::mutex> CS(s_HashMapMutex);

			auto iter = s_ComputePSOHashMap.find(HashCode);

			if (iter == s_ComputePSOHashMap.end())
			{
				firstCompile = true;
				PSORef = s_ComputePSOHashMap[HashCode].GetAddressOf();
			}
			else
			{
				PSORef = iter->second.GetAddressOf();
			}
		}

		if (firstCompile)
		{
			
			PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateComputePipelineState(&m_PSODesc, IID_PPV_ARGS(m_pPSO.GetAddressOf())) >= 0,
				"Create Pipeline State Object Error!");

			s_GraphicsPSOHashMap[HashCode] = m_pPSO;
			m_pPSO->SetName(m_Name);
		}
		else
		{
			while (*PSORef == nullptr)
				std::this_thread::yield();
			m_pPSO = *PSORef;
		}
	}

	void ComputePSO::SetDepthTargetFormat(ImageFormat DSVFormat, uint32_t MsaaCount /*= 1*/, uint32_t MsaaQuality /*= 0*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void ComputePSO::SetRenderTargetFormat(ImageFormat RTVFormat, ImageFormat DSVFormat, uint32_t MsaaCount /*= 1*/, uint32_t MsaaQuality /*= 0*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void ComputePSO::SetRenderTargetFormats(uint32_t NumRTVs, const ImageFormat* RTVFormats, ImageFormat DSVFormat, uint32_t MsaaCount /*= 1*/, uint32_t MsaaQuality /*= 0*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void ComputePSO::SetVertexShader(const void* Binary, size_t Size)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void ComputePSO::SetPixelShader(const void* Binary, size_t Size)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}