#include "pxpch.h"

#include "GenerateMips.h"

#include "Platform/DirectX/PipelineStateObject/DirectXPipelineStateObject.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorHeap.h"
#include "Platform/DirectX/PipelineStateObject/DirectXRootSignature.h"
#include "Platform/DirectX/Sampler/SamplerManager.h"
#include "Platform/DirectX/PipelineStateObject/DirectXShader.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorAllocator.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorHandle.h"
#include "Platform/DirectX/Texture/DirectXCubeTexture.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/Context/DirectXContextManager.h"

namespace Pixel {

	GenerateMips::GenerateMips()
	{
		//initialize root signature
		m_RootSignature = RootSignature::Create(3, 1);
		(*m_RootSignature)[(uint32_t)GenerateMipsRootBinds::GenerateMipsCB].InitAsConstants(0, sizeof(GenerateMipsCB) / 4);
		(*m_RootSignature)[(uint32_t)GenerateMipsRootBinds::SrcMip].InitAsDescriptorRange(RangeType::SRV, 0, 1, ShaderVisibility::ALL);
		(*m_RootSignature)[(uint32_t)GenerateMipsRootBinds::OutMip].InitAsDescriptorRange(RangeType::UAV, 0, 4, ShaderVisibility::ALL);

		Ref<SamplerDesc> pSampleDesc = SamplerDesc::Create();
		pSampleDesc->SetTextureAddressMode(AddressMode::CLAMP);
		pSampleDesc->SetFilter(Filter::MIN_MAG_MIP_LINEAR);
		m_RootSignature->InitStaticSampler(0, pSampleDesc, ShaderVisibility::ALL);

		m_RootSignature->Finalize(L"GenerateMipMaps", RootSignatureFlag::AllowInputAssemblerInputLayout);
		m_PipelineState = CreateRef<ComputePSO>(L"GenerateMipMaps");

		m_GenerateMipMapsCS = Shader::Create("assets/shaders/IBL/GenerateMipsCS.hlsl", "CS", "cs_5_0");
		auto [CsBinary, CsBinarySize] = std::static_pointer_cast<DirectXShader>(m_GenerateMipMapsCS)->GetShaderBinary();
		m_PipelineState->SetComputeShader(CsBinary, CsBinarySize);

		m_UAVDescriptorHandle = DescriptorAllocator::AllocateCpuAndGpuDescriptorHandle(DescriptorHeapType::CBV_UAV_SRV, 4);

		std::vector<DescriptorHandle> m_tempHandles;
		m_tempHandles.resize(4);

		uint32_t DescriptorSize = Device::Get()->GetDescriptorAllocator((uint32_t)DescriptorHeapType::CBV_UAV_SRV)->GetDescriptorSize();

		for (uint32_t i = 0; i < 4; ++i)
		{
			m_tempHandles[i] = (*m_UAVDescriptorHandle) + i * DescriptorSize;
		}

		//create descriptor
		for (uint32_t i = 0; i < 4; ++i)
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			uavDesc.Texture2D.MipSlice = i;//mip levels
			uavDesc.Texture2D.PlaneSlice = 0;

			std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreateUnorderedAccessView(
				nullptr, nullptr, &uavDesc, std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_tempHandles[i].GetCpuHandle())->GetCpuHandle()
			);
		}

		m_PipelineState->Finalize();
	}

	Ref<CubeTexture> GenerateMips::GenerateCubeMapMips(Ref<CubeTexture> pCubeTexture)
	{
		return nullptr;
	}

}