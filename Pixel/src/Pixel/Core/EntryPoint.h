#pragma once

#define _CRTDBG_MAP_ALLOC
#ifdef PX_DEBUG
#include <crtdbg.h>
#endif

#ifndef PX_OPENGL
#include <wrl/client.h>
#include <dxgi1_3.h>
#include <dxgidebug.h>
#include "Platform/DirectX/d3dx12.h"
#endif

#ifdef PX_PLATFORM_WINDOWS

extern Pixel::Application* Pixel::CreateApplication();

int main()
{
	Pixel::Log::Init();
	//printf("Pixel Engine\n");

	PX_PROFILE_BEGIN_SESSION("Startup", "PixelProfile-Startup.json");
	auto app = Pixel::CreateApplication();
	PX_PROFILE_END_SESSION();
	PX_PROFILE_BEGIN_SESSION("Runtime", "PixelProfile-Runtime.json");
	app->Run();
	PX_PROFILE_END_SESSION();
	PX_PROFILE_BEGIN_SESSION("Startup", "PixelProfile-Shutdown.json");
	delete app;
	PX_PROFILE_END_SESSION();

#if defined(_DEBUG)
	{
		Microsoft::WRL::ComPtr<IDXGIDebug1> pdxgiDebug;
		if (DXGIGetDebugInterface1(0, IID_PPV_ARGS(pdxgiDebug.ReleaseAndGetAddressOf())) >= 0)
		{
			pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_ALL));
		}
	}
#endif	
}

#endif
