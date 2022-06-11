#pragma once

#include "pxpch.h"

#include "Pixel/Core/Core.h"
#include "Pixel/Events/Event.h"

namespace Pixel {

	class Device;

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "Pixel Engine",
			uint32_t width = 1280,
			uint32_t height = 720)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
	class PIXEL_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetCursorNormal() = 0;
		virtual void SetCursorDisabled() = 0;
		virtual void SetCursorPos(int32_t x, int32_t y) = 0;
		virtual void SetCursorViewPortCenter() = 0;
		virtual void SetViewPortCenterPoint(int32_t x, int32_t y) = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual Ref<Device> GetDevice() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};

}
