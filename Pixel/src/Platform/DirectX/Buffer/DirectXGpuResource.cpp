#include "pxpch.h"

#include "DirectXGpuResource.h"

#include "Platform/DirectX/d3dx12.h"
#include "Platform/DirectX/TypeUtils.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"

namespace Pixel {
	
	DirectXGpuResource::DirectXGpuResource():m_GpuVirtualAddress(0), m_UsageState(D3D12_RESOURCE_STATE_COMMON), m_TransitioningState((D3D12_RESOURCE_STATES)-1)
	{

	}

	DirectXGpuResource::DirectXGpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState)
		:m_GpuVirtualAddress(0),
		m_pResource(pResource),
		m_UsageState(CurrentState),
		m_TransitioningState((D3D12_RESOURCE_STATES)-1)
	{

	}

	DirectXGpuResource::DirectXGpuResource(ResourceStates CurrentState)
	{
		m_UsageState = ResourceStatesToDirectXResourceStates(CurrentState);
	}

	DirectXGpuResource::~DirectXGpuResource()
	{

	}

	void DirectXGpuResource::Destroy()
	{
		m_pResource = nullptr;
		m_GpuVirtualAddress = 0;
		++m_VersionID;
	}

	void DirectXGpuResource::SetResource(void* resource)
	{
		m_pResource = (ID3D12Resource*)resource;
	}

	Ref<DescriptorCpuHandle> DirectXGpuResource::GetUAV() const
	{
		return std::make_shared<DirectXDescriptorCpuHandle>();
	}

	Ref<DescriptorCpuHandle> DirectXGpuResource::GetSRV() const
	{
		return std::make_shared<DirectXDescriptorCpuHandle>();
	}

	void DirectXGpuResource::CreateFromSwapChain(const std::wstring& Name)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}