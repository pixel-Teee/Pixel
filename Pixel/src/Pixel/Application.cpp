#include "pxpch.h"
#include "Application.h"

#include "Pixel/Events/ApplicationEvent.h"
#include "Pixel/Log.h"

namespace Pixel {
	Application::Application()
	{

	}
	Application::~Application()
	{

	}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication))
		{
			PIXEL_TRACE(e);
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			PIXEL_TRACE(e);
		}

		while (true);
	}
}
