#pragma once

#include <wrl/client.h>
#include "Platform/DirectX/d3dx12.h"

#include "Pixel/Renderer/Buffer/GpuResource.h"

namespace Pixel {
	class GpuVirtualAddress;
	class DirectXGpuResource : public GpuResource
	{
		friend class DirectXContext;
		friend class GraphicsContext;
		friend class DirectXGpuBuffer;
		friend class DirectXByteAddressBuffer;
		friend class DirectXPixelBuffer;
		friend class DepthBuffer;
	public:
		DirectXGpuResource();

		DirectXGpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState);

		virtual ~DirectXGpuResource();

		virtual void Destroy() override;

		ID3D12Resource* operator->(){ return m_pResource.Get();}
		const ID3D12Resource* operator->() const { return m_pResource.Get(); }

		ID3D12Resource* GetResource() { return m_pResource.Get(); }
		const ID3D12Resource* GetResource() const { return m_pResource.Get(); }

		Ref<GpuVirtualAddress> GetGpuVirtualAddress() const { return m_GpuVirtualAddress; }

		uint32_t GetVersionID() const { return m_VersionID; }
	protected:

		Microsoft::WRL::ComPtr<ID3D12Resource> m_pResource;
		//------Usage State------
		D3D12_RESOURCE_STATES m_UsageState;
		//------Usage State------
		
		//------Transition State------
		D3D12_RESOURCE_STATES m_TransitioningState;
		//------Transition State------

		Ref<GpuVirtualAddress> m_GpuVirtualAddress;

		//used to identify when a resource changes
		//so descriptors can be copied etc
		uint32_t m_VersionID = 0;
	};
}
