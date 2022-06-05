#include "pxpch.h"

#include "DirectXGpuResource.h"

#include "d3dx12.h"

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

	DirectXGpuResource::~DirectXGpuResource()
	{

	}

	void DirectXGpuResource::Destroy()
	{
		m_pResource = nullptr;
		m_GpuVirtualAddress = 0;
		++m_VersionID;
	}

}