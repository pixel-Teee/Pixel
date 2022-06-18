#pragma once

#include "Platform/DirectX/Context/DirectXContext.h"

namespace Pixel {
	class DescriptorGpuHandle;
	class DirectXDescriptorGpuHandle;
	class GpuVirtualAddress;
	class ComputeContext : public DirectXContext
	{
	public:
		ComputeContext(CommandListType type);
		//directx gpu buffer
		void ClearUAV(GpuResource& Target);

		//directx color buffer
		void ClearUAV(GpuResource& Target, glm::vec4 ClearColor);

		virtual void SetRootSignature(const RootSignature& RootSig) override;

		virtual void SetConstantArray(uint32_t RootIndex, uint32_t NumConstants, const void* pConstants) override;

		virtual void SetConstant(uint32_t RootIndex, uint32_t Offset, uint32_t Val) override;

		virtual void SetConstants(uint32_t RootIndex, uint32_t x) override;

		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y) override;

		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z) override;

		virtual void SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z, uint32_t w) override;

		virtual void SetConstantBuffer(uint32_t RootIndex, Ref<GpuVirtualAddress> CBV) override;

		virtual void SetDynamicConstantBufferView(uint32_t RootIndex, size_t BufferSize, const void* BufferData) override;

		virtual void SetDynamicSRV(uint32_t RootIndex, size_t BufferSize, const void* BufferData) override;

		virtual void SetBufferSRV(uint32_t RootIndex, const GpuResource& SRV, uint64_t Offset = 0) override;

		virtual void SetBufferUAV(uint32_t RootIndex, const GpuResource& UAV, uint64_t Offset = 0) override;

		virtual void SetDescriptorTable(uint32_t RootIndex, Ref<DescriptorGpuHandle> FirstHandle) override;

		virtual void Dispatch(size_t GroupCountX = 1, size_t GroupCountY = 1, size_t GroupCountZ = 1) override;

		virtual void Dispatch1D(size_t ThreadCountX, size_t GroupSizeX = 64) override;

		virtual void Dispatch2D(size_t ThreadCountX, size_t ThreadCountY, size_t GroupSizeX = 8, size_t GroupSizeY = 8) override;
	};
}
