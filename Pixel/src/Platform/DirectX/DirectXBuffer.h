#pragma once

#include "GpuResource.h"

namespace Pixel {

	//normal gpu buffer
	class GpuBuffer : public GpuResource
	{
	public:
		virtual ~GpuBuffer();

		const D3D12_CPU_DESCRIPTOR_HANDLE& GetUAV() const { return m_UAV; }
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() const { return m_SRV; }

		//create a buffer. if initial data is provided, it will be copied into the buffer
		void Create(const std::wstring& name, uint32_t NumElements, uint32_t ElementSize, const void* initialData = nullptr);

		//root descriptor, cound directly used as virtual address
		D3D12_GPU_VIRTUAL_ADDRESS RootConstantBufferView() const { return m_GpuVirtualAddress; }

		D3D12_CPU_DESCRIPTOR_HANDLE CreateConstantBufferView(uint32_t Offset, uint32_t Size) const;

		//vertex buffer view and index buffer view
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView(size_t Offset, uint32_t Size, uint32_t Stride) const;
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView(size_t BaseVertexIndex = 0) const;

		D3D12_INDEX_BUFFER_VIEW IndexBufferView(size_t Offset, uint32_t Size, bool b32Bit = false) const;
		D3D12_INDEX_BUFFER_VIEW IndexBufferView(SIZE_T StartIndex = 0) const;
		//vertex buffer view and index buffer view

		size_t GetBufferSize() const { return m_BufferSize; }
		uint32_t GetElementCount() const { return m_ElementCount; }
		uint32_t GetElementSize() const { return m_ElementSize; }
	protected:

		GpuBuffer();

		D3D12_RESOURCE_DESC DescribeBuffer();
		virtual void CreateDerivedViews() = 0;

		//unordered access view
		D3D12_CPU_DESCRIPTOR_HANDLE m_UAV;
		//shader resource view
		D3D12_CPU_DESCRIPTOR_HANDLE m_SRV;

		//buffer size
		size_t m_BufferSize;

		//element count
		uint32_t m_ElementCount;

		//element size
		uint32_t m_ElementSize;

		//resource flag
		D3D12_RESOURCE_FLAGS m_ResourceFlags;
	};

	class ByteAddressBuffer : public GpuBuffer
	{
	public:
		virtual void CreateDerivedViews() override;
	};
}