#include "pxpch.h"
#include "DescriptorAllocator.h"

#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorAllocator.h"

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

	Ref<DescriptorCpuHandle> DescriptorAllocator::AllocateDescriptor(DescriptorHeapType Type, uint32_t Count /*= 1*/, Ref<Device> pDevice)
	{
		return g_Descriptor[(uint64_t)Type]->Allocate(Count, pDevice);
	}

	void DescriptorAllocator::DestroyDescriptorPools()
	{
		for (uint32_t i = 0; i < 4; ++i)
		{
			g_Descriptor[i]->DestroyDescriptorPools();
		}
	}

	Ref<DescriptorAllocator> DescriptorAllocator::g_Descriptor[4] = {
		DescriptorAllocator::Create(DescriptorHeapType::CBV_UAV_SRV),
		DescriptorAllocator::Create(DescriptorHeapType::SAMPLER),
		DescriptorAllocator::Create(DescriptorHeapType::RTV),
		DescriptorAllocator::Create(DescriptorHeapType::DSV)
	};

}