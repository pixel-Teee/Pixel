#pragma once

#ifdef PX_PLATFORM_WINDOWS

extern Pixel::Application* Pixel::CreateApplication();

int main()
{
	Pixel::Log::Init();
	PIXEL_CORE_WARN("Initialized Log!");
	int a = 5;
	PIXEL_INFO("Hello! Var = {0}", a);
	printf("Pixel Engine\n");
	auto app = Pixel::CreateApplication();
	app->Run();
	delete app;
}

#endif
