#pragma once

#include <wrl/client.h>
#include <dxgi1_4.h>

#include "d3dx12.h"

#include <string>

#include "Pixel/Renderer/Device/Device.h"

struct GLFWwindow;

namespace Pixel {
	class DirectXSwapChain;
	class ContextManager;
	class CommandListManager;
	class DescriptorAllocator;
	class GpuResource;

	class DirectXDevice : public Device
	{
	public:
		//DirectXDevice(GLFWwindow* windowHandle);
		DirectXDevice();
		virtual ~DirectXDevice();

		void Initialize();
		void SetWindowHandle(GLFWwindow* windowHandle);
		//static Ref<DirectXDevice> Get();

		Microsoft::WRL::ComPtr<ID3D12Device1> GetDevice();
		Microsoft::WRL::ComPtr<IDXGIFactory4> GetDxgiFactory();

		Ref<CommandListManager> GetCommandListManager();
		virtual Ref<ContextManager> GetContextManager() override;

		//------set and get client size------
		virtual void SetClientSize(uint32_t width, uint32_t height) override;
		std::pair<uint32_t, uint32_t> GetClientSize();
		//------set and get client size------

		//------get render target descriptor size------
		uint32_t GetRtvDescriptorSize();
		//------get render target descriptor size------

		//------4x msaa------
		void Set4xMsaaState(bool value);
		bool Get4xMsaaState();
		uint64_t Get4xMsaaQuality();
		//------4x msaa------

		HWND GetNativeWindowHandle();

		//------Get Buffer Format------
		DXGI_FORMAT GetBackBufferFormat();
		DXGI_FORMAT GetDepthBufferFormat();
		//------Get Buffer Format------

		virtual void ReCreateSwapChain() override;
		Ref<DirectXSwapChain> GetSwapChain();

		Ref<DescriptorAllocator> GetDescriptorAllocator(uint32_t index);

		virtual void CopyDescriptorsSimple(uint32_t NumDescriptors, Ref<DescriptorCpuHandle> DestHandle, Ref<DescriptorCpuHandle> SrcHandle, DescriptorHeapType Type) override;

		virtual Ref<GpuResource> GetCurrentBackBuffer() override;

	private:
		//------output hard device------
		void LogAdapters();
		void LogAdapterOutputs(IDXGIAdapter* adapter);
		void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
		std::wstring GetLatestWinPixGpuCapturePath();
		//------output hard device------

		//------descriptor size------
		uint32_t m_RtvDescriptorSize = 0;
		uint32_t m_DsvDescriptorSize = 0;
		//const buffer view/shader resource view/unordered access view
		uint32_t m_CbvSrvUavDescriptorSize = 0;
		//------descriptor size------

		GLFWwindow* m_pWindowHandle;

		Microsoft::WRL::ComPtr<IDXGIFactory4> m_pDxgiFactory;
		Microsoft::WRL::ComPtr<ID3D12Device1> m_pD3dDevice;

		//back buffer format and depth stencil buffer format
		DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;//[0, 1]
		DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;//unsigned 32 bit

		bool m_4xMassState = false;//4x msaa switch
		uint64_t m_4xMsaaQuality = 0;//quality level of 4x msaa

		//TODO:need to refractor to swap chain class
		uint32_t m_ClientWidth, m_ClientHeight;

		Ref<DirectXSwapChain> m_pSwapChain;

		Ref<CommandListManager> m_pCommandListManager;

		Ref<ContextManager> m_pContextManager;

		Ref<DescriptorAllocator> m_DescriptorAllocator[4];
		//static Ref<DirectXDevice> m_gpDevice;
	};
}
