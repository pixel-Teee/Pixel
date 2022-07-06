#include "pxpch.h"

#include "d3dx12.h"

#include "DirectXSwapChain.h"
#include "DirectXDevice.h"
#include "Platform/DirectX/Command/CommandQueue.h"
#include "Platform/DirectX/DirectXDevice.h"

namespace Pixel {

	DirectXSwapChain::DirectXSwapChain(Ref<Device> device)
	{
		CreateSwapChain(device);

		CreateRtvAndDsvDescriptorHeaps(device);
	}

	void DirectXSwapChain::OnResize(Ref<Device> pDevice)
	{
		//Ref<Device> pDevice = std::make_shared<DirectXDevice>(&device);
		std::static_pointer_cast<DirectXDevice>(pDevice)->GetCommandListManager()->IdleGPU();

		//release the previous resources we will be recreating
		for (size_t i = 0; i < SwapChainBufferCount; ++i)
			m_SwapChainBuffer[i].Reset();

		//resize the swap chain
		PX_CORE_ASSERT(
			m_SwapChain->ResizeBuffers(SwapChainBufferCount, std::static_pointer_cast<DirectXDevice>(pDevice)->GetClientSize().first, std::static_pointer_cast<DirectXDevice>(pDevice)->GetClientSize().second,
				std::static_pointer_cast<DirectXDevice>(pDevice)->GetBackBufferFormat(), DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH) >= 0,
			"resize the swap chain error!"
		);

		m_CurrentBackBufferIndex = 1;

		//create the swap chain buffer, and create rtv references id3d12resource
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (size_t i = 0; i < SwapChainBufferCount; ++i)
		{
			PX_CORE_ASSERT(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])) >= 0, "Get The Swap Chain Buffer Error!");
			std::static_pointer_cast<DirectXDevice>(pDevice)->GetDevice()->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHandle);
			rtvHandle.ptr += std::static_pointer_cast<DirectXDevice>(pDevice)->GetRtvDescriptorSize();
		}

		std::static_pointer_cast<DirectXDevice>(pDevice)->GetCommandListManager()->IdleGPU();
	}

	void DirectXSwapChain::CreateSwapChain(Ref<Device> device)
	{
		m_SwapChain.Reset();

		//describe the swap chain information
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc.Width = std::static_pointer_cast<DirectXDevice>(device)->GetClientSize().first;
		swapChainDesc.BufferDesc.Height = std::static_pointer_cast<DirectXDevice>(device)->GetClientSize().second;//width and height

		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;//refresh rate

		swapChainDesc.BufferDesc.Format = std::static_pointer_cast<DirectXDevice>(device)->GetBackBufferFormat();//back buffer format

		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDesc.SampleDesc.Count = std::static_pointer_cast<DirectXDevice>(device)->Get4xMsaaState() ? 4 : 1;
		swapChainDesc.SampleDesc.Quality = std::static_pointer_cast<DirectXDevice>(device)->Get4xMsaaState() ? (std::static_pointer_cast<DirectXDevice>(device)->Get4xMsaaQuality() - 1) : 0;

		//------usage------
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		//------usage------

		swapChainDesc.BufferCount = SwapChainBufferCount;

		//------output window------
		swapChainDesc.OutputWindow = std::static_pointer_cast<DirectXDevice>(device)->GetNativeWindowHandle();
		//------output window------

		swapChainDesc.Windowed = true;

		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		//Ref<Device> pDevice = std::make_shared<DirectXDevice>(&device);

		PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(device)->GetDxgiFactory()->CreateSwapChain(
			std::static_pointer_cast<DirectXDevice>(device)->GetCommandListManager()->GetGraphicsQueue().GetNativeCommandQueue().Get(),
			&swapChainDesc,
			m_SwapChain.ReleaseAndGetAddressOf()
		) >= 0, "Create Swap Chain Error!");
	}

	uint32_t DirectXSwapChain::GetSwapChainBufferCount()
	{
		return SwapChainBufferCount;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectXSwapChain::DepthStencilView()
	{
		return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectXSwapChain::GetRtvHandle(uint32_t index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE RtvHandle;
		RtvHandle.ptr = m_rtvHeap->GetCPUDescriptorHandleForHeapStart().ptr + index * std::static_pointer_cast<DirectXDevice>(Device::Get())->GetRtvDescriptorSize();
		return RtvHandle;
	}

	uint32_t DirectXSwapChain::GetCurrentBackBufferIndex()
	{
		return m_CurrentBackBufferIndex;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> DirectXSwapChain::GetCurrentBackBufferSource()
	{
		return m_SwapChainBuffer[m_CurrentBackBufferIndex];
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> DirectXSwapChain::GetBackBufferSource(uint32_t index)
	{
		return m_SwapChainBuffer[index];
	}

	void DirectXSwapChain::Present()
	{
		m_SwapChain->Present(1, 0);
		
		m_CurrentBackBufferIndex = (m_CurrentBackBufferIndex + 1) % SwapChainBufferCount;
	}

	void DirectXSwapChain::CreateRtvAndDsvDescriptorHeaps(Ref<Device> device)
	{
		//render target view descriptor heap
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvHeapDesc.NodeMask = 0;

		PX_CORE_ASSERT(std::static_pointer_cast<DirectXDevice>(device)->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc,
		IID_PPV_ARGS(m_rtvHeap.ReleaseAndGetAddressOf())) >= 0, "Create RTV Descriptor Heap Error!");
	}

}