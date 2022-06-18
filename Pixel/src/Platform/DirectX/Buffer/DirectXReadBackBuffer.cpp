#include "pxpch.h"

#include "DirectXReadBackBuffer.h"

#include "Platform/DirectX/DirectXDevice.h"

#include "Platform/DirectX/TypeUtils.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"

namespace Pixel {

	void ReadBackBuffer::Create(const std::wstring& name, uint32_t NumElements, uint32_t ElementSize)
	{
		Destroy();

		m_ElementCount = NumElements;
		m_ElementSize = ElementSize;
		m_BufferSize = NumElements * ElementSize;
		m_UsageState = D3D12_RESOURCE_STATE_COPY_DEST;


		//create a readback buffer large enough to hold all texel data
		D3D12_HEAP_PROPERTIES HeapProps;
		HeapProps.Type = D3D12_HEAP_TYPE_READBACK;
		HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProps.CreationNodeMask = 1;
		HeapProps.VisibleNodeMask = 1;

		//read back buffers must be 1-dimensional
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

		PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice()->CreateCommittedResource(&HeapProps,
		D3D12_HEAP_FLAG_NONE, &ResourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(m_pResource.GetAddressOf())) >= 0,
			"Create Read Back Resource Error!");

#ifdef PX_DEBUG
		m_pResource->SetName(name.c_str());
#else
		(name);
#endif
	}

	void* ReadBackBuffer::Map()
	{
		void* Memory;
		m_pResource->Map(0, &CD3DX12_RANGE(0, m_BufferSize), &Memory);
		return Memory;
	}

	void ReadBackBuffer::UnMap()
	{
		m_pResource->Unmap(0, &CD3DX12_RANGE(0, 0));
	}

}