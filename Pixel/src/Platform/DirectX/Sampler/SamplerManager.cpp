#include "pxpch.h"

#include "SamplerManager.h"
#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorAllocator.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Pixel/Utils/Hash.h"

namespace Pixel {

	std::map<size_t, D3D12_CPU_DESCRIPTOR_HANDLE> s_SamplerCache;

	DirectXSamplerDesc::DirectXSamplerDesc()
	{
		m_SamplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
		m_SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		m_SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		m_SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		m_SamplerDesc.MipLODBias = 0.0f;
		m_SamplerDesc.MaxAnisotropy = 16;
		m_SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		m_SamplerDesc.BorderColor[0] = 1.0f;
		m_SamplerDesc.BorderColor[1] = 1.0f;
		m_SamplerDesc.BorderColor[2] = 1.0f;
		m_SamplerDesc.BorderColor[3] = 1.0f;
		m_SamplerDesc.MinLOD = 0.0F;
		m_SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	}

	void DirectXSamplerDesc::SetTextureAddressMode(D3D12_TEXTURE_ADDRESS_MODE AddressMode)
	{
		m_SamplerDesc.AddressU = AddressMode;
		m_SamplerDesc.AddressV = AddressMode;
		m_SamplerDesc.AddressW = AddressMode;
	}

	void DirectXSamplerDesc::SetBoarderColor(glm::vec4 BorderColor)
	{
		m_SamplerDesc.BorderColor[0] = BorderColor.r;
		m_SamplerDesc.BorderColor[1] = BorderColor.g;
		m_SamplerDesc.BorderColor[2] = BorderColor.b;
		m_SamplerDesc.BorderColor[3] = BorderColor.a;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectXSamplerDesc::CreateDescriptor()
	{
		size_t hashValue = Utility::HashState(&m_SamplerDesc);
		auto iter = s_SamplerCache.find(hashValue);
		if (iter != s_SamplerCache.end())
		{
			return iter->second;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(DescriptorAllocator::AllocateDescriptor(DescriptorHeapType::SAMPLER, 1))->GetCpuHandle();
		std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateSampler(&m_SamplerDesc, handle);
		return handle;
	}



} 