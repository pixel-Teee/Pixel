#pragma once

#include <wrl/client.h>
#include "d3dx12.h"

namespace Pixel {
	class GpuResource
	{
		friend class DirectXContext;
	public:
		GpuResource();

		GpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState);

		~GpuResource();

		virtual void Destroy();

		ID3D12Resource* operator->(){ return m_pResource.Get();}
		const ID3D12Resource* operator->() const { return m_pResource.Get(); }

		ID3D12Resource* GetResource() { return m_pResource.Get(); }
		const ID3D12Resource* GetResource() const { return m_pResource.Get(); }

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return m_GpuVirtualAddress; }

		uint32_t GetVersionID() const { return m_VersionID; }
	protected:

		Microsoft::WRL::ComPtr<ID3D12Resource> m_pResource;
		//------Usage State------
		D3D12_RESOURCE_STATES m_UsageState;
		//------Usage State------
		
		//------Transition State------
		D3D12_RESOURCE_STATES m_TransitioningState;
		//------Transition State------

		D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;

		//used to identify when a resource changes
		//so descriptors can be copied etc
		uint32_t m_VersionID = 0;
	};
}
