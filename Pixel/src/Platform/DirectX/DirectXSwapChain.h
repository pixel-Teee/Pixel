#pragma once

#include "DirectXDevice.h"

namespace Pixel {
	class DirectXSwapChain
	{
	public:
		DirectXSwapChain(DirectXDevice& device);
		void OnResize(DirectXDevice& device);

		//if you change the swap chain's 4x msaa state, you should call this function
		void CreateSwapChain(DirectXDevice& device);

		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView();
	private:
		static constexpr uint32_t SwapChainBufferCount = 2;

		void CreateRtvAndDsvDescriptorHeaps(DirectXDevice& device);

		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;

		//render target view
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;

		//depth stencil view
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[SwapChainBufferCount];

		int32_t m_CurrentBackBufferIndex = 0;
	};
}
