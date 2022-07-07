#include "pxpch.h"

#include "SamplerManager.h"
#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorAllocator.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Platform/DirectX/TypeUtils.h"

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

	void DirectXSamplerDesc::SetTextureAddressMode(AddressMode addressMode)
	{
		m_SamplerDesc.AddressU = AddressModeToDirectXAddressMode(addressMode);
		m_SamplerDesc.AddressV = AddressModeToDirectXAddressMode(addressMode);
		m_SamplerDesc.AddressW = AddressModeToDirectXAddressMode(addressMode);
	}

	void DirectXSamplerDesc::SetFilter(Filter filter)
	{
		m_SamplerDesc.Filter = FilterToDirectXFilter(filter);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectXSamplerDesc::CreateDescriptor()
	{
		size_t hashValue = Utility::HashState((uint32_t*)&m_SamplerDesc);
		auto iter = s_SamplerCache.find(hashValue);
		if (iter != s_SamplerCache.end())
		{
			return iter->second;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE handle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(DescriptorAllocator::AllocateDescriptor(DescriptorHeapType::SAMPLER, 1))->GetCpuHandle();
		std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateSampler(&m_SamplerDesc, handle);
		return handle;
	}



	void DirectXSamplerDesc::SetBorderColor(glm::vec4 BoarderColor)
	{
		m_SamplerDesc.BorderColor[0] = BoarderColor.x;
		m_SamplerDesc.BorderColor[1] = BoarderColor.y;
		m_SamplerDesc.BorderColor[2] = BoarderColor.z;
		m_SamplerDesc.BorderColor[3] = BoarderColor.w;
	}

} 