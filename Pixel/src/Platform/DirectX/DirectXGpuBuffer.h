#pragma once

#include "Pixel/Renderer/GpuBuffer.h"
#include "Pixel/Renderer/DescriptorCpuHandle.h"

#include "DirectXGpuResource.h"

namespace Pixel {
	class GpuVirtualAddress;
	class IBV;
	class VBV;
	//normal gpu buffer
	class DirectXGpuBuffer : public GpuBuffer
	{
	public:
		friend class DirectXContext;
		friend class GraphicsContext;
		DirectXGpuBuffer();
		virtual ~DirectXGpuBuffer();

		virtual Ref<DescriptorCpuHandle> GetUAV() const override { return m_UAV; }
		virtual Ref<DescriptorCpuHandle> GetSRV() const override { return m_SRV; }

		//create a buffer. if initial data is provided, it will be copied into the buffer
		void Create(const std::wstring& name, uint32_t NumElements, uint32_t ElementSize, const void* initialData = nullptr);

		//root descriptor, cound directly used as virtual address
		Ref<GpuVirtualAddress> RootConstantBufferView() const { return m_GpuResource.m_GpuVirtualAddress; }

		Ref<DescriptorCpuHandle> CreateConstantBufferView(uint32_t Offset, uint32_t Size) const;

		//vertex buffer view and index buffer view
		Ref<VBV> VertexBufferView(size_t Offset, uint32_t Size, uint32_t Stride) const;
		Ref<VBV> VertexBufferView(size_t BaseVertexIndex = 0) const;

		Ref<IBV> IndexBufferView(size_t Offset, uint32_t Size, bool b32Bit = false) const;
		Ref<IBV> IndexBufferView(SIZE_T StartIndex = 0) const;
		//vertex buffer view and index buffer view

		size_t GetBufferSize() const { return m_BufferSize; }
		uint32_t GetElementCount() const { return m_ElementCount; }
		uint32_t GetElementSize() const { return m_ElementSize; }
	protected:

		D3D12_RESOURCE_DESC DescribeBuffer();
		virtual void CreateDerivedViews();

		//unordered access view
		Ref<DescriptorCpuHandle> m_UAV;
		//shader resource view
		Ref<DescriptorCpuHandle> m_SRV;

		//buffer size
		size_t m_BufferSize;

		//element count
		uint32_t m_ElementCount;

		//element size
		uint32_t m_ElementSize;

		//resource flag
		D3D12_RESOURCE_FLAGS m_ResourceFlags;

		DirectXGpuResource m_GpuResource;
	};

	class DirectXByteAddressBuffer : public DirectXGpuBuffer
	{
	public:
		virtual void CreateDerivedViews() override;
	};
}