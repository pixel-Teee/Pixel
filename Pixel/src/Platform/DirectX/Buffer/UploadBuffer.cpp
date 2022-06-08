#include "pxpch.h"

#include "UploadBuffer.h"

#include "Platform/DirectX/DirectXDevice.h"
#include "DirectXGpuVirtualAddress.h"

namespace Pixel {

	void UploadBuffer::Create(const std::wstring& name, size_t BufferSize)
	{
		Destroy();

		//create an unpload buffer
		//this is cpu-visible, but it's write combined memory, so avoid reading back from it
		D3D12_HEAP_PROPERTIES HeapProps;
		HeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProps.CreationNodeMask = 1;
		HeapProps.VisibleNodeMask = 1;

		//upload buffers must be 1-dimensional
		D3D12_RESOURCE_DESC ResourceDesc = {};
		ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		ResourceDesc.Width = m_BufferSize;
		ResourceDesc.Height = 1;
		ResourceDesc.DepthOrArraySize = 1;
		ResourceDesc.MipLevels = 1;
		ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		ResourceDesc.SampleDesc.Count = 1;
		ResourceDesc.SampleDesc.Quality = 0;
		ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		PX_CORE_ASSERT(DirectXDevice::Get()->GetDevice()->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE,
			&ResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pResource)) >= 0, "create resource error!");

		Ref<GpuVirtualAddress> pGpuVirtualAddress = std::make_shared<DirectXGpuVirtualAddress>();

		std::static_pointer_cast<DirectXGpuVirtualAddress>(pGpuVirtualAddress)->SetGpuVirtualAddress(m_pResource->GetGPUVirtualAddress());
		//m_GpuVirtualAddress = m_pResource->GetGPUVirtualAddress();
		m_GpuVirtualAddress = pGpuVirtualAddress;

#ifdef PX_DEBUG
		m_pResource->SetName(name.c_str());

#else
		(name);
#endif
	}

	void* UploadBuffer::Map()
	{
		void* Memory;
		m_pResource->Map(0, &CD3DX12_RANGE(0, m_BufferSize), &Memory);
		return Memory;
	}

	void UploadBuffer::UnMap(size_t begin, size_t end /*= -1*/)
	{
		m_pResource->Unmap(0, &CD3DX12_RANGE(begin, std::min(end, m_BufferSize)));
	}

}