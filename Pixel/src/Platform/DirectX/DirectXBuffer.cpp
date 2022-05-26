#include "pxpch.h"

#include "d3dx12.h"
#include "Pixel/Math/Math.h"

#include "DirectXBuffer.h"
#include "DirectXDevice.h"
#include "DescriptorAllocator.h"

namespace Pixel {

	GpuBuffer::~GpuBuffer()
	{
		Destroy();
	}

	void GpuBuffer::Create(const std::wstring& name, uint32_t NumElements, uint32_t ElementSize, const void* initialData /*= nullptr*/)
	{
		Destroy();

		m_ElementCount = NumElements;
		m_ElementSize = ElementSize;
		m_BufferSize = NumElements * ElementSize;

		//get a buffer create describe
		D3D12_RESOURCE_DESC ResourceDesc = DescribeBuffer();

		m_UsageState = D3D12_RESOURCE_STATE_COMMON;

		D3D12_HEAP_PROPERTIES HeapProps;
		HeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		HeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		HeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		HeapProps.CreationNodeMask = 1;
		HeapProps.VisibleNodeMask = 1;

		//Create Buffer Resource
		PX_CORE_ASSERT(DirectXDevice::Get()->GetDevice()->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, m_UsageState,
			nullptr, IID_PPV_ARGS(&m_pResource)) >= 0, "Create Default Resource Error!");

		m_GpuVirtualAddress = m_pResource->GetGPUVirtualAddress();

		if (initialData)
		{
			//TODO:copy the data into the buffer
		}

#ifdef PX_DEBUG
		m_pResource->SetName(name.c_str());
#else
		(name);
#endif

		
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GpuBuffer::CreateConstantBufferView(uint32_t Offset, uint32_t Size) const
	{
		PX_CORE_ASSERT(Offset + Size <= m_BufferSize, "Out of the buffer range!");

		//align up to 16
		Size = Math::AlignUp(Size, 16);

		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		CBVDesc.BufferLocation = m_GpuVirtualAddress + (size_t)Offset;
		CBVDesc.SizeInBytes = Size;

		D3D12_CPU_DESCRIPTOR_HANDLE hCBV = DescriptorAllocator::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);

		DirectXDevice::Get()->GetDevice()->CreateConstantBufferView(&CBVDesc, hCBV);

		return hCBV;
	}

	D3D12_VERTEX_BUFFER_VIEW GpuBuffer::VertexBufferView(size_t Offset, uint32_t Size, uint32_t Stride) const
	{
		D3D12_VERTEX_BUFFER_VIEW VBView;
		VBView.BufferLocation = m_GpuVirtualAddress + Offset;
		VBView.SizeInBytes = Size;
		VBView.StrideInBytes = Stride;
		return VBView;
	}

	D3D12_VERTEX_BUFFER_VIEW GpuBuffer::VertexBufferView(size_t BaseVertexIndex) const
	{
		size_t Offset = BaseVertexIndex + m_ElementSize;
		return VertexBufferView(Offset, (uint32_t)(m_BufferSize - Offset), m_ElementSize);
	}

	D3D12_INDEX_BUFFER_VIEW GpuBuffer::IndexBufferView(size_t Offset, uint32_t Size, bool b32Bit /*= false*/) const
	{
		D3D12_INDEX_BUFFER_VIEW IBView;
		IBView.BufferLocation = m_GpuVirtualAddress + Offset;
		IBView.Format = b32Bit ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
		IBView.SizeInBytes = Size;
		return IBView;
	}

	D3D12_INDEX_BUFFER_VIEW GpuBuffer::IndexBufferView(SIZE_T StartIndex) const
	{
		size_t Offset = StartIndex * m_ElementSize;
		//if m_ElementSize == 4, then 32bit, else 16bit
		return IndexBufferView(Offset, (uint32_t)(m_BufferSize - Offset), m_ElementSize == 4);
	}

	GpuBuffer::GpuBuffer()
		:m_BufferSize(0), m_ElementCount(0), m_ElementSize(0)
	{
		m_ResourceFlags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		m_UAV.ptr = -1;
		m_SRV.ptr = -1;
	}

	D3D12_RESOURCE_DESC GpuBuffer::DescribeBuffer()
	{
		PX_CORE_ASSERT(m_BufferSize != 0, "BufferSize is 0!");

		D3D12_RESOURCE_DESC Desc = {};
		Desc.Alignment = 0;
		Desc.DepthOrArraySize = 1;
		Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		Desc.Flags = m_ResourceFlags;
		Desc.Format = DXGI_FORMAT_UNKNOWN;
		Desc.Height = 1;
		Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		Desc.MipLevels = 1;
		Desc.SampleDesc.Count = 1;
		Desc.SampleDesc.Quality = 0;
		//important
		Desc.Width = (uint64_t)m_BufferSize;

		return Desc;
	}

}