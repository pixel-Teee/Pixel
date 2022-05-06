#pragma once

#define _CRTDBG_MAP_ALLOC
#ifdef PX_DEBUG
#include <crtdbg.h>
#include <vld.h>
#endif

#ifdef PX_PLATFORM_WINDOWS

extern Pixel::Application* Pixel::CreateApplication();

int main()
{
#ifdef PX_DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(761);
#endif
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
#ifdef PX_DEBUG
	_CrtDumpMemoryLeaks();
#endif
}

#endif
