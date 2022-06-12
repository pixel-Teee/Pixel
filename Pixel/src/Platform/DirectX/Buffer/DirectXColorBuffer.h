#pragma once

#include <glm/glm.hpp>

#include "Platform/DirectX/d3dx12.h"
#include "DirectXPixelBuffer.h"

namespace Pixel {
	class Device;
	class DescriptorCpuHandle;
	class DirectXColorBuffer : public DirectXPixelBuffer
	{
	public:
		friend class GraphicsContext;
		DirectXColorBuffer(glm::vec4 ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

		//create a color buffer from a swap chain buffer
		//unordered access is restricted
		virtual void CreateFromSwapChain(const std::wstring& Name) override;

		//create a color buffer
		//if an address is supplied, memory will not be allocated
		//the video memory address allows you to alias buffers(which can be especially userful for reusing ESRAM across a frame)
		void Create(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t NumMips,
			ImageFormat Format, Ref<GpuVirtualAddress> VideoMemoryPtr);

		//create a color buffer
		//memory will be allocated in esram(on xbox one)
		//on windows, this functions the same as create() without a video address
		//void Create(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t NumMips,
		//	DXGI_FORMAT Format);

		//create a color buffer
		//if an address is supplied, memory will not be allocated
		//the video memory address allows you to alias buffers(which can be especially userful for reusing ESRAM across a frame)
		void CreateArray(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t ArrayCount,
			ImageFormat Format, Ref<GpuVirtualAddress> VideoMemoryPtr);

		//create a color buffer
		//memory will be allocated in esram(on xbox one)
		//on windows, this functions the same as create() without a video address
		//void CreateArray(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t ArrayCount,
		//	DXGI_FORMAT Format);

		//get pre-created cpu-visible descriptor handles
		virtual Ref<DescriptorCpuHandle> GetSRV() const { return m_SRVHandle; }
		virtual Ref<DescriptorCpuHandle> GetRTV() const { return m_RTVHandle; }
		virtual Ref<DescriptorCpuHandle> GetUAV() const { return m_UAVHandle[0]; }

		void SetClearColor(glm::vec4 ClearColor) { m_ClearColor = ClearColor; }

		void SetMsaaMode(uint32_t NumColorSamples, uint32_t NumConverageSamples);

		glm::vec4 GetClearColor() const { return m_ClearColor; }

		//this will work for all texture sizes, but it's recommended for speed and quality
		//that you use dimensions with powers of two (but not necessarily square)
		//pass 0 for array count to reserve for mips at creation time
		void GenerateMipMaps(DirectXContext& context);
	protected:

		D3D12_RESOURCE_FLAGS CombineResourceFlags() const;

		static inline uint32_t ComputeNumMips(uint32_t Width, uint32_t Height);

		void CreateDerivedViews(ImageFormat Format, uint32_t ArraySize, uint32_t NumMips);

		glm::vec4 m_ClearColor;

		Ref<DescriptorCpuHandle> m_SRVHandle;
		Ref<DescriptorCpuHandle> m_RTVHandle;
		Ref<DescriptorCpuHandle> m_UAVHandle[12];

		uint32_t m_NumMipMaps;//number of texture sublevels
		uint32_t m_FragmentCount;
		uint32_t m_SampleCount;
	};
}