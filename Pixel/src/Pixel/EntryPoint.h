#pragma once

#ifdef PX_PLATFORM_WINDOWS

extern Pixel::Application* Pixel::CreateApplication();

int main()
{
	printf("Pixel Engine");
	auto app = Pixel::CreateApplication();
	app->Run();
	delete app;
}

#endif
