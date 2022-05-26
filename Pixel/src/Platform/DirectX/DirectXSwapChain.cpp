#include "pxpch.h"

#include "d3dx12.h"

#include "DirectXSwapChain.h"
#include "DirectXDevice.h"
#include "CommandQueue.h"

namespace Pixel {

	DirectXSwapChain::DirectXSwapChain(DirectXDevice& device)
	{
		CreateSwapChain(device);

		CreateRtvAndDsvDescriptorHeaps(device);
	}

	void DirectXSwapChain::OnResize(DirectXDevice& device)
	{
		CommandListManager::Get()->IdleGPU();

		//release the previous resources we will be recreating
		for (size_t i = 0; i < SwapChainBufferCount; ++i)
			m_SwapChainBuffer[i].Reset();

		//resize the swap chain
		PX_CORE_ASSERT(
			m_SwapChain->ResizeBuffers(SwapChainBufferCount, device.GetClientSize().first, device.GetClientSize().second,
				device.GetBackBufferFormat(), DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH) >= 0,
			"resize the swap chain error!"
		);

		m_CurrentBackBufferIndex = 0;

		//create the swap chain buffer, and create rtv references id3d12resource
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (size_t i = 0; i < SwapChainBufferCount; ++i)
		{
			PX_CORE_ASSERT(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])) >= 0, "Get The Swap Chain Buffer Error!");
			device.GetDevice()->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHandle);
		}

		CommandListManager::Get()->IdleGPU();
	}

	void DirectXSwapChain::CreateSwapChain(DirectXDevice& device)
	{
		m_SwapChain.Reset();

		//describe the swap chain information
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc.Width = device.GetClientSize().first;
		swapChainDesc.BufferDesc.Height = device.GetClientSize().second;//width and height

		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;//refresh rate

		swapChainDesc.BufferDesc.Format = device.GetBackBufferFormat();//back buffer format

		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDesc.SampleDesc.Count = device.Get4xMsaaState() ? 4 : 1;
		swapChainDesc.SampleDesc.Quality = device.Get4xMsaaState() ? (device.Get4xMsaaQuality() - 1) : 0;

		//------usage------
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		//------usage------

		swapChainDesc.BufferCount = SwapChainBufferCount;

		//------output window------
		swapChainDesc.OutputWindow = device.GetNativeWindowHandle();
		//------output window------

		swapChainDesc.Windowed = true;

		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		PX_CORE_ASSERT(device.GetDxgiFactory()->CreateSwapChain(
			CommandListManager::Get()->GetGraphicsQueue().GetNativeCommandQueue().Get(),
			&swapChainDesc,
			m_SwapChain.ReleaseAndGetAddressOf()
		) >= 0, "Create Swap Chain Error!");
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectXSwapChain::DepthStencilView()
	{
		return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
	}

	void DirectXSwapChain::CreateRtvAndDsvDescriptorHeaps(DirectXDevice& device)
	{
		//render target view descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;

		PX_CORE_ASSERT(device.GetDevice()->CreateDescriptorHeap(&rtvHeapDesc,
		IID_PPV_ARGS(m_rtvHeap.ReleaseAndGetAddressOf())) >= 0, "Create RTV Descriptor Heap Error!");
	}

}