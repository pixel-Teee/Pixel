#include "pxpch.h"
#include "DirectXDevice.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#if defined(DEBUG) || defined(_DEBUG)
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#include <ShlObj.h>
#include <strsafe.h>
#endif

#include "DirectXSwapChain.h"

namespace Pixel {

	DirectXDevice::DirectXDevice(GLFWwindow* windowHandle)
	{
		m_pWindowHandle = windowHandle;
	}

	DirectXDevice::~DirectXDevice()
	{

	}

	void DirectXDevice::Initialize()
	{
#if defined(DEBUG) || defined(_DEBUG)
		//enable the d3d12 debug layer
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		PX_CORE_ASSERT(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.ReleaseAndGetAddressOf())) >= 0, "get debug layer error!");
		debugController->EnableDebugLayer();

		Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
		if (DXGIGetDebugInterface1(0, IID_PPV_ARGS(pDxgiInfoQueue.ReleaseAndGetAddressOf())) >= 0)
		{
			pDxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			pDxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);
		}

		//PIX
		if (GetModuleHandle(L"WinPixGpuCapturer.dll") == 0)
		{
			LoadLibrary(GetLatestWinPixGpuCapturePath().c_str());
		}
#endif
		PX_CORE_ASSERT(CreateDXGIFactory1(IID_PPV_ARGS(m_pDxgiFactory.ReleaseAndGetAddressOf())) >= 0, "get dxgi factory error!");

		//try to create hardware device
		HRESULT hardWareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_pD3dDevice.ReleaseAndGetAddressOf()));

		//fallback to create software device
		if (hardWareResult < 0)
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
			PX_CORE_ASSERT(m_pDxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(pWarpAdapter.ReleaseAndGetAddressOf())) >= 0, "don't have sofaware wrapper");

			//from the pWarpAdapter to get the m_pD3dDevice
			PX_CORE_ASSERT(
				D3D12CreateDevice(
					pWarpAdapter.Get(),
					D3D_FEATURE_LEVEL_11_0,
					IID_PPV_ARGS(&m_pD3dDevice)) >= 0, "get the sofaware wrapper error!");
		}

		//------get the descriptor size------
		m_RtvDescriptorSize = m_pD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_DsvDescriptorSize = m_pD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		m_CbvSrvUavDescriptorSize = m_pD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		//------get the descriptor size------

		//------check 4x msaa quality for our back buffer format------
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = m_BackBufferFormat;
		msQualityLevels.SampleCount = 4;//sample count
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		PX_CORE_ASSERT(m_pD3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels))
			>= 0, "check feature error");

		m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
		PX_CORE_ASSERT(m_4xMsaaQuality > 0, "unexpected msaa quality level!");
		//------check 4x msaa quality for our back buffer format------

#if defined(DEBUG) || defined(_DEBUG)
		LogAdapters();
#endif
		//------Create Swap Chain------
		m_pSwapChain = CreateRef<DirectXSwapChain>(*this);

		//recreate the swap chain buffer rtv handle and depth stencil handle
		m_pSwapChain->OnResize(*this);
		//------Create Swap Chain------
	}

	Ref<DirectXDevice> DirectXDevice::Get()
	{
		return m_gpDevice;
	}

	Microsoft::WRL::ComPtr<ID3D12Device> DirectXDevice::GetDevice()
{
		return m_pD3dDevice;
	}

	Microsoft::WRL::ComPtr<IDXGIFactory4> DirectXDevice::GetDxgiFactory()
	{
		return m_pDxgiFactory;
	}

	void DirectXDevice::SetClientSize(uint32_t width, uint32_t height)
	{
		m_ClientWidth = width;
		m_ClientHeight = height;
	}

	std::pair<uint32_t, uint32_t> DirectXDevice::GetClientSize()
	{
		return std::make_pair(m_ClientWidth, m_ClientHeight);
	}

	uint32_t DirectXDevice::GetRtvDescriptorSize()
	{
		return m_RtvDescriptorSize;
	}

	void DirectXDevice::Set4xMsaaState(bool value)
	{
		if (m_4xMassState != value)
		{
			m_4xMassState = value;

			//recreate the swap chain and buffers with new multisample settings

			//recreate the swap chain
			m_pSwapChain->CreateSwapChain(*this);
			m_pSwapChain->OnResize(*this);
		}
	}

	bool DirectXDevice::Get4xMsaaState()
	{
		return m_4xMassState;
	}

	uint64_t DirectXDevice::Get4xMsaaQuality()
	{
		return m_4xMsaaQuality;
	}

	HWND DirectXDevice::GetNativeWindowHandle()
	{
		return glfwGetWin32Window(m_pWindowHandle);
	}

	//------Get Buffer Format------
	DXGI_FORMAT DirectXDevice::GetBackBufferFormat()
	{
		return m_BackBufferFormat;
	}

	DXGI_FORMAT DirectXDevice::GetDepthBufferFormat()
	{
		return m_DepthStencilFormat;
	}
	//------Get Buffer Format------

	void DirectXDevice::LogAdapters()
	{
		//enum adapters
		uint32_t i = 0;
		IDXGIAdapter* adapter = nullptr;

		std::vector<IDXGIAdapter*> adapterList;
		while (m_pDxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			//description
			DXGI_ADAPTER_DESC desc;
			adapter->GetDesc(&desc);

			std::wstring text = L"***Adapter: ";
			text += desc.Description;

			PIXEL_CORE_INFO(L"{0}", text);

			adapterList.push_back(adapter);

			++i;
		}

		for (size_t i = 0; i < adapterList.size(); ++i)
		{
			LogAdapterOutputs(adapterList[i]);

			//------Release ComPtr------
			if (adapterList[i] != nullptr) {
				adapterList[i]->Release();
				adapterList[i] = nullptr;
			}
			//------Release ComPtr------
		}
	}

	void DirectXDevice::LogAdapterOutputs(IDXGIAdapter* adapter)
	{
		uint32_t i = 0;
		IDXGIOutput* output = nullptr;
		while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_OUTPUT_DESC desc;
			output->GetDesc(&desc);

			std::wstring text = L"***Output: ";
			text += desc.DeviceName;

			PIXEL_CORE_INFO(L"{0}", text);

			LogOutputDisplayModes(output, m_BackBufferFormat);

			//------Release ComPtr------
			if (output != nullptr)
			{
				output->Release();
				output = nullptr;
			}
			//------Release ComPtr------

			++i;
		}
	}

	void DirectXDevice::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
	{
		uint32_t count = 0;
		uint32_t flags = 0;

		//call with nullptr to get list count, similar to vulkan
		output->GetDisplayModeList(format, flags, &count, nullptr);

		std::vector<DXGI_MODE_DESC> modeList(count);
		output->GetDisplayModeList(format, flags, &count, &modeList[0]);

		for (auto& mode : modeList)
		{
			uint32_t n = mode.RefreshRate.Numerator;
			uint32_t d = mode.RefreshRate.Denominator;

			std::wstring text =
				L"Width = " + std::to_wstring(mode.Width) + L" " +
				L"Height = " + std::to_wstring(mode.Height) + L" " +
				L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
				L"\n";

			PIXEL_CORE_INFO(L"{0}", text);
		}
	}

	
	std::wstring DirectXDevice::GetLatestWinPixGpuCapturePath()
	{
		LPWSTR programFilesPath = nullptr;
		SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath);

		std::wstring pixSearchPath = programFilesPath + std::wstring(L"\\Microsoft PIX\\*");

		WIN32_FIND_DATA findData;
		bool foundPixInstallation = false;
		wchar_t newestVersionFound[MAX_PATH];

		HANDLE hFind = FindFirstFile(pixSearchPath.c_str(), &findData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) &&
					(findData.cFileName[0] != '.'))
				{
					if (!foundPixInstallation || wcscmp(newestVersionFound, findData.cFileName) <= 0)
					{
						foundPixInstallation = true;
						StringCchCopy(newestVersionFound, _countof(newestVersionFound), findData.cFileName);
					}
				}
			} while (FindNextFile(hFind, &findData) != 0);
		}

		FindClose(hFind);

		if (!foundPixInstallation)
		{
			// TODO: Error, no PIX installation found
		}

		wchar_t output[MAX_PATH];
		StringCchCopy(output, pixSearchPath.length(), pixSearchPath.data());
		StringCchCat(output, MAX_PATH, &newestVersionFound[0]);
		StringCchCat(output, MAX_PATH, L"\\WinPixGpuCapturer.dll");

		return &output[0];
	}

	Ref<DirectXDevice> DirectXDevice::m_gpDevice;

}