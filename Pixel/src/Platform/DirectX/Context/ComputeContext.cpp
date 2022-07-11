#include "pxpch.h"

#include "ComputeContext.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/DirectX/Buffer/DirectXColorBuffer.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorGpuHandle.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Platform/DirectX/PipelineStateObject/DirectXRootSignature.h"
#include "Platform/DirectX/Buffer/DirectXGpuVirtualAddress.h"
#include "Platform/DirectX/Buffer/DirectXGpuBuffer.h"
#include "Pixel/Math/Math.h"

namespace Pixel {

	ComputeContext::ComputeContext(CommandListType type)
		:DirectXContext(type)
	{

	}

	void ComputeContext::ClearUAV(GpuResource& Target)
	{
		FlushResourceBarriers();

		//after binding a uav
		//we can get a gpu handle that is required to clear it as a uav
		Ref<DescriptorGpuHandle> GpuVisibileHandle = m_DynamicViewDescriptorHeap.UploadDirect(Target.GetUAV());

		D3D12_GPU_DESCRIPTOR_HANDLE NativeGpuVisibleHandle = std::static_pointer_cast<DirectXDescriptorGpuHandle>(GpuVisibileHandle)->GetGpuHandle();

		D3D12_CPU_DESCRIPTOR_HANDLE NativeCpuHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(Target.GetUAV())->GetCpuHandle();
	
		const uint32_t ClearColor[4] = {};

		Microsoft::WRL::ComPtr<ID3D12Resource> pResource = static_cast<DirectXGpuResource&>(Target).GetComPtrResource();
		m_pCommandList->ClearUnorderedAccessViewUint(NativeGpuVisibleHandle, NativeCpuHandle, pResource.Get(), ClearColor, 0, nullptr);
	}

	void ComputeContext::ClearUAV(GpuResource& Target, glm::vec4 ClearColor)
	{
		FlushResourceBarriers();

		//after binding a uav
		//we can get a gpu handle that is required to clear it as a uav
		Ref<DescriptorGpuHandle> GpuVisibileHandle = m_DynamicViewDescriptorHeap.UploadDirect(Target.GetUAV());

		DirectXColorBuffer& refColorBuffer = static_cast<DirectXColorBuffer&>(Target);

		CD3DX12_RECT ClearRect(0, 0, (LONG)refColorBuffer.GetWidth(), (LONG)refColorBuffer.GetHeight());

		D3D12_GPU_DESCRIPTOR_HANDLE NativeGpuVisibleHandle = std::static_pointer_cast<DirectXDescriptorGpuHandle>(GpuVisibileHandle)->GetGpuHandle();

		D3D12_CPU_DESCRIPTOR_HANDLE NativeCpuHandle = std::static_pointer_cast<DirectXDescriptorCpuHandle>(Target.GetUAV())->GetCpuHandle();

		//const uint32_t ClearColor[4] = {};

		Microsoft::WRL::ComPtr<ID3D12Resource> pResource = static_cast<DirectXGpuResource&>(Target).GetComPtrResource();
		m_pCommandList->ClearUnorderedAccessViewFloat(NativeGpuVisibleHandle, NativeCpuHandle, pResource.Get(), glm::value_ptr(ClearColor), 1, &ClearRect);
	}

	void ComputeContext::SetRootSignature(const RootSignature& RootSig)
	{
		DirectXRootSignature& refRootSignature = const_cast<DirectXRootSignature&>(static_cast<const DirectXRootSignature&>(RootSig));

		//PX_CORE_ASSERT(m_CurrComputeRootSingature == refRootSignature.GetComPtrSignature(),
		//	"current's compute rootsignature is not qual to parameter's rootsignature!");

		m_CurrComputeRootSingature = refRootSignature.GetComPtrSignature();

		m_pCommandList->SetComputeRootSignature(m_CurrComputeRootSingature.Get());

		m_DynamicViewDescriptorHeap.ParseComputeRootSignature(RootSig);
		m_DynamicSamplerDescriptorHeap.ParseComputeRootSignature(RootSig);
	}

	void ComputeContext::SetConstantArray(uint32_t RootIndex, uint32_t NumConstants, const void* pConstants)
	{
		m_pCommandList->SetComputeRoot32BitConstants(RootIndex, NumConstants, pConstants, 0);
	}

	void ComputeContext::SetConstantArray(uint32_t RootIndex, uint32_t NumConstants, const void* pConstants, uint32_t offset)
	{
		m_pCommandList->SetComputeRoot32BitConstants(RootIndex, NumConstants, pConstants, offset);
	}

	void ComputeContext::SetConstant(uint32_t RootIndex, uint32_t Offset, uint32_t Val)
	{
		m_pCommandList->SetComputeRoot32BitConstant(RootIndex, Val, Offset);
	}

	void ComputeContext::SetConstants(uint32_t RootIndex, uint32_t x)
	{
		m_pCommandList->SetComputeRoot32BitConstant(RootIndex, x, 0);
	}

	void ComputeContext::SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y)
	{
		m_pCommandList->SetComputeRoot32BitConstant(RootIndex, x, 0);
		m_pCommandList->SetComputeRoot32BitConstant(RootIndex, y, 1);
	}

	void ComputeContext::SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z)
	{
		m_pCommandList->SetComputeRoot32BitConstant(RootIndex, x, 0);
		m_pCommandList->SetComputeRoot32BitConstant(RootIndex, y, 1);
		m_pCommandList->SetComputeRoot32BitConstant(RootIndex, z, 2);
	}

	void ComputeContext::SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z, uint32_t w)
	{
		m_pCommandList->SetComputeRoot32BitConstant(RootIndex, x, 0);
		m_pCommandList->SetComputeRoot32BitConstant(RootIndex, y, 1);
		m_pCommandList->SetComputeRoot32BitConstant(RootIndex, z, 2);
		m_pCommandList->SetComputeRoot32BitConstant(RootIndex, w, 3);
	}

	void ComputeContext::SetConstantBuffer(uint32_t RootIndex, Ref<GpuVirtualAddress> CBV)
	{
		Ref<DirectXGpuVirtualAddress> pVirtualAddress = std::static_pointer_cast<DirectXGpuVirtualAddress>(CBV);
		m_pCommandList->SetComputeRootConstantBufferView(RootIndex, pVirtualAddress->GetGpuVirtualAddress());
	}

	void ComputeContext::SetDynamicConstantBufferView(uint32_t RootIndex, size_t BufferSize, const void* BufferData)
	{
		PX_CORE_ASSERT(BufferData != nullptr && Math::IsAligned(BufferData, 16), "buffer data is not aligned!");

		//allocate video memory
		DynAlloc cb = m_CpuLinearAllocator.Allocate(BufferSize, 256);

		memcpy(cb.DataPtr, BufferData, BufferSize);

		m_pCommandList->SetComputeRootConstantBufferView(RootIndex, cb.GpuAddress);
	}

	void ComputeContext::SetDynamicSRV(uint32_t RootIndex, size_t BufferSize, const void* BufferData)
	{
		PX_CORE_ASSERT(BufferData != nullptr && Math::IsAligned(BufferData, 16), "buffer data is not aligned!");

		DynAlloc cb = m_CpuLinearAllocator.Allocate(BufferSize, 256);

		memcpy(cb.DataPtr, BufferData, Math::AlignUp(BufferSize, 16));

		m_pCommandList->SetComputeRootShaderResourceView(RootIndex, cb.GpuAddress);
	}

	void ComputeContext::SetBufferSRV(uint32_t RootIndex, const GpuResource& SRV, uint64_t Offset /*= 0*/)
	{
		//Ref<>
		const DirectXGpuBuffer& refSRV = static_cast<const DirectXGpuBuffer&>(SRV);

		PX_CORE_ASSERT((refSRV.m_UsageState & D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE) != 0,
			"resource state error!");

		D3D12_GPU_VIRTUAL_ADDRESS virtualAddress = std::static_pointer_cast<DirectXGpuVirtualAddress>(refSRV.GetGpuVirtualAddress())->GetGpuVirtualAddress();

		m_pCommandList->SetComputeRootShaderResourceView(RootIndex, virtualAddress + Offset);
	}

	void ComputeContext::SetBufferUAV(uint32_t RootIndex, const GpuResource& UAV, uint64_t Offset /*= 0*/)
	{
		const DirectXGpuBuffer& refUAV = static_cast<const DirectXGpuBuffer&>(UAV);

		PX_CORE_ASSERT((refUAV.m_UsageState & D3D12_RESOURCE_STATE_UNORDERED_ACCESS) != 0,
			"resource state error!");

		D3D12_GPU_VIRTUAL_ADDRESS virtualAddress = std::static_pointer_cast<DirectXGpuVirtualAddress>(refUAV.GetGpuVirtualAddress())->GetGpuVirtualAddress();

		m_pCommandList->SetComputeRootUnorderedAccessView(RootIndex, virtualAddress + Offset);
	}

	void ComputeContext::SetDescriptorTable(uint32_t RootIndex, Ref<DescriptorGpuHandle> FirstHandle)
	{
		Ref<DirectXDescriptorGpuHandle> gpuHandle = std::static_pointer_cast<DirectXDescriptorGpuHandle>(FirstHandle);
		m_pCommandList->SetComputeRootDescriptorTable(RootIndex, gpuHandle->GetGpuHandle());
	}

	void ComputeContext::Dispatch(size_t GroupCountX /*= 1*/, size_t GroupCountY /*= 1*/, size_t GroupCountZ /*= 1*/)
	{
		FlushResourceBarriers();
		m_DynamicViewDescriptorHeap.CommitComputeRootDescriptorTables(m_pCommandList.Get());
		m_DynamicSamplerDescriptorHeap.CommitComputeRootDescriptorTables(m_pCommandList.Get());
		m_pCommandList->Dispatch((uint32_t)GroupCountX, (uint32_t)GroupCountY, (uint32_t)GroupCountZ);
	}

	void ComputeContext::Dispatch1D(size_t ThreadCountX, size_t GroupSizeX /*= 64*/)
	{
		Dispatch(Math::DivideByMultiple(ThreadCountX, GroupSizeX), 1, 1);
	}

	void ComputeContext::Dispatch2D(size_t ThreadCountX, size_t ThreadCountY, size_t GroupSizeX /*= 8*/, size_t GroupSizeY /*= 8*/)
	{
		Dispatch(Math::DivideByMultiple(ThreadCountX, GroupSizeX),
			Math::DivideByMultiple(ThreadCountY, GroupSizeY), 1);
	}

}