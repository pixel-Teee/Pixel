#pragma once

#include "DirectXDevice.h"

namespace Pixel {
	class Device;
	class DirectXSwapChain
	{
	public:
		DirectXSwapChain(Ref<Device> device);
		void OnResize(Ref<Device> pDevice);

		//if you change the swap chain's 4x msaa state, you should call this function
		void CreateSwapChain(Ref<Device> device);

		uint32_t GetSwapChainBufferCount();

		D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView();

		D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle(uint32_t index);

		uint32_t GetCurrentBackBufferIndex();

		Microsoft::WRL::ComPtr<ID3D12Resource> GetCurrentBackBufferSource();

		Microsoft::WRL::ComPtr<ID3D12Resource> GetBackBufferSource(uint32_t index);		

		void Present();
	private:
		static constexpr uint32_t SwapChainBufferCount = 2;

		void CreateRtvAndDsvDescriptorHeaps(Ref<Device> device);

		Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;

		//render target view
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;

		//depth stencil view
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

		Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[SwapChainBufferCount];

		int32_t m_CurrentBackBufferIndex = 0;
	};
}
