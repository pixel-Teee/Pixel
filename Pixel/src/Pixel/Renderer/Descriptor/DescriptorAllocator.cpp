#include "pxpch.h"
#include "DescriptorAllocator.h"

#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorAllocator.h"
#include "Platform/DirectX/DirectXDevice.h"

namespace Pixel {

	Ref<DescriptorAllocator> DescriptorAllocator::Create(DescriptorHeapType HeapType)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: PX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::DirectX12: return std::make_shared<DirectXDescriptorAllocator>(HeapType);
		}

		PX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<DescriptorCpuHandle> DescriptorAllocator::AllocateDescriptor(DescriptorHeapType Type, uint32_t Count /*= 1*/)
	{
		return Device::Get()->GetDescriptorAllocator((uint32_t)Type)->Allocate(Count);
	}

	Ref<DescriptorHandle> DescriptorAllocator::AllocateCpuAndGpuDescriptorHandle(DescriptorHeapType Type, uint32_t Count)
	{
		return Device::Get()->GetDescriptorAllocator((uint32_t)Type)->AllocateCpuAndGpuHandle(Count);
	}

	//void DescriptorAllocator::DestroyDescriptorPools()
	//{
	//	for (uint32_t i = 0; i < 4; ++i)
	//	{
	//		g_Descriptor[i]->DestroyDescriptor();
	//	}
	//}

}