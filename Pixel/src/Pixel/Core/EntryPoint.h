#pragma once

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
}

#endif
