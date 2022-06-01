#pragma once

#include <glm/glm.hpp>

#include "PixelBuffer.h"

namespace Pixel {
	class ColorBuffer : public PixelBuffer
	{
	public:
		ColorBuffer(glm::vec4 ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

		//create a color buffer from a swap chain buffer
		//unordered access is restricted
		void CreateFromSwapChain(const std::wstring& Name, ID3D12Resource* pBaseResouce);

		//create a color buffer
		//if an address is supplied, memory will not be allocated
		//the video memory address allows you to alias buffers(which can be especially userful for reusing ESRAM across a frame)
		void Create(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t NumMips,
			DXGI_FORMAT Format, D3D12_GPU_VIRTUAL_ADDRESS VideoMemoryPtr = -1);

		//create a color buffer
		//memory will be allocated in esram(on xbox one)
		//on windows, this functions the same as create() without a video address
		//void Create(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t NumMips,
		//	DXGI_FORMAT Format);

		//create a color buffer
		//if an address is supplied, memory will not be allocated
		//the video memory address allows you to alias buffers(which can be especially userful for reusing ESRAM across a frame)
		void CreateArray(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t ArrayCount,
			DXGI_FORMAT Format, D3D12_GPU_VIRTUAL_ADDRESS VideoMemoryPtr = -1);

		//create a color buffer
		//memory will be allocated in esram(on xbox one)
		//on windows, this functions the same as create() without a video address
		//void CreateArray(const std::wstring& Name, uint32_t Width, uint32_t Height, uint32_t ArrayCount,
		//	DXGI_FORMAT Format);

		//get pre-created cpu-visible descriptor handles
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() const { return m_SRVHandle; }
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTV() const { return m_RTVHandle; }
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetUAV() const { return m_UAVHandle[0]; }

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

		void CreateDerivedViews(DXGI_FORMAT Format, uint32_t ArraySize, uint32_t NumMips = 1);

		glm::vec4 m_ClearColor;

		D3D12_CPU_DESCRIPTOR_HANDLE m_SRVHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE m_UAVHandle[12];

		uint32_t m_NumMipMaps;//number of texture sublevels
		uint32_t m_FragmentCount;
		uint32_t m_SampleCount;
	};
}