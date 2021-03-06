#include "pxpch.h"

#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Math/Math.h"

#include "DirectXGpuBuffer.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorAllocator.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "DirectXGpuVirtualAddress.h"
#include "Platform/DirectX/View/DirectXIndexBufferView.h"
#include "Platform/DirectX/View/DirectXVertexBufferView.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/Context/DirectXContextManager.h"
#include "Platform/DirectX/Context/GraphicsContext.h"

namespace Pixel {

	DirectXGpuBuffer::~DirectXGpuBuffer()
	{
		
	}

	void DirectXGpuBuffer::Create(const std::wstring& name, uint32_t NumElements, uint32_t ElementSize, const void* initialData)
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
		PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &ResourceDesc, m_UsageState,
			nullptr, IID_PPV_ARGS(m_pResource.GetAddressOf())) >= 0, "Create Default Resource Error!");

		//std::static_pointer_cast<DirectXGpuResource>(m_pResource)->m_GpuVirtualAddress = std::static_pointer_cast<DirectXGpuVirtualAddress>(std::static_pointer_cast<DirectXGpuResource>(m_pResource)->GetGpuVirtualAddress())->GetGpuVirtualAddress();
		m_GpuVirtualAddress = std::make_shared<DirectXGpuVirtualAddress>();
		std::static_pointer_cast<DirectXGpuVirtualAddress>(m_GpuVirtualAddress)->SetGpuVirtualAddress(m_pResource->GetGPUVirtualAddress());

		if (initialData)
		{
			Ref<Context> pContext = DirectXDevice::Get()->GetContextManager()->AllocateContext(CommandListType::Graphics);
			std::static_pointer_cast<GraphicsContext>(pContext)->InitializeBuffer(*this, initialData, m_BufferSize, 0);
		}

#ifdef PX_DEBUG
		m_pResource->SetName(name.c_str());
#else
		(name);
#endif

		
	}

	Ref<DescriptorCpuHandle> DirectXGpuBuffer::CreateConstantBufferView(uint32_t Offset, uint32_t Size) const
	{
		PX_CORE_ASSERT(Offset + Size <= m_BufferSize, "Out of the buffer range!");

		//align up to 16
		Size = Math::AlignUp(Size, 16);

		D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
		CBVDesc.BufferLocation = m_pResource->GetGPUVirtualAddress() + (size_t)Offset;
		CBVDesc.SizeInBytes = Size;

		D3D12_CPU_DESCRIPTOR_HANDLE hCBV = std::static_pointer_cast<DirectXDescriptorCpuHandle>(DescriptorAllocator::AllocateDescriptor(DescriptorHeapType::CBV_UAV_SRV, 1))->GetCpuHandle();

		std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateConstantBufferView(&CBVDesc, hCBV);

		Ref<DescriptorCpuHandle> pCpuHandle = std::make_shared<DirectXDescriptorCpuHandle>();
		std::static_pointer_cast<DirectXDescriptorCpuHandle>(pCpuHandle)->SetCpuHandle(hCBV);

		return pCpuHandle;
	}

	Ref<VBV> DirectXGpuBuffer::VertexBufferView(size_t Offset, uint32_t Size, uint32_t Stride) const
	{
		Ref<VBV> pVbv;
		pVbv = std::make_shared<DirectXVBV>(m_GpuVirtualAddress, Offset, Size, Stride);
		return pVbv;
	}

	Ref<VBV> DirectXGpuBuffer::VertexBufferView(size_t BaseVertexIndex) const
	{
		size_t Offset = BaseVertexIndex + m_ElementSize;
		return VertexBufferView(Offset, (uint32_t)(m_BufferSize - Offset), m_ElementSize);
	}

	Ref<IBV> DirectXGpuBuffer::IndexBufferView(size_t Offset, uint32_t Size, bool b32Bit /*= false*/) const
	{
		Ref<IBV> pIbv;
		pIbv = std::make_shared<DirectXIBV>(m_GpuVirtualAddress, Offset, Size, b32Bit);
		return pIbv;
	}

	Ref<IBV> DirectXGpuBuffer::IndexBufferView(SIZE_T StartIndex) const
	{
		size_t Offset = StartIndex * m_ElementSize;
		//if m_ElementSize == 4, then 32bit, else 16bit
		return IndexBufferView(Offset, (uint32_t)(m_BufferSize - Offset), m_ElementSize == 4);
	}

	DirectXGpuBuffer::DirectXGpuBuffer()
		:m_BufferSize(0), m_ElementCount(0), m_ElementSize(0)
	{
		m_ResourceFlags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		D3D12_CPU_DESCRIPTOR_HANDLE UnknownHandle;
		UnknownHandle.ptr = -1;

		m_UAV = DescriptorCpuHandle::Create();
		m_SRV = DescriptorCpuHandle::Create();

		std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_UAV)->SetCpuHandle(UnknownHandle);
		std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_SRV)->SetCpuHandle(UnknownHandle);
	}

	D3D12_RESOURCE_DESC DirectXGpuBuffer::DescribeBuffer()
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

	void DirectXGpuBuffer::CreateDerivedViews()
	{

	}

	void DirectXByteAddressBuffer::CreateDerivedViews()
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		SRVDesc.Format = DXGI_FORMAT_R32_TYPELESS;//non type
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.Buffer.NumElements = (uint32_t)m_BufferSize / 4;
		SRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

		//TODO:need to simplify
		if (std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_SRV)->GetCpuHandle().ptr == -1)
			std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_UAV)->SetCpuHandle(std::static_pointer_cast<DirectXDescriptorCpuHandle>(DirectXDescriptorAllocator::AllocateDescriptor(DescriptorHeapType::CBV_UAV_SRV, 1))->GetCpuHandle());
		std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateShaderResourceView(m_pResource.Get(), &SRVDesc, std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_SRV)->GetCpuHandle());

		D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
		UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		UAVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		UAVDesc.Buffer.NumElements = (uint32_t)m_BufferSize / 4;
		UAVDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

		Ref<DirectXDescriptorCpuHandle> TempUavHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(m_UAV);

		if(TempUavHandle->GetCpuHandle().ptr == -1)
			TempUavHandle->SetCpuHandle(std::static_pointer_cast<DirectXDescriptorCpuHandle>(DirectXDescriptorAllocator::AllocateDescriptor(DescriptorHeapType::CBV_UAV_SRV, 1))->GetCpuHandle());
		std::static_pointer_cast<DirectXDevice>(DirectXDevice::Get())->GetDevice()->CreateUnorderedAccessView(m_pResource.Get(), nullptr, &UAVDesc, TempUavHandle->GetCpuHandle());
	}

}