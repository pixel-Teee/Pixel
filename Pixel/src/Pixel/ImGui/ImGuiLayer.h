#pragma once

#include "Pixel/Core/Layer.h"

#include "Pixel/Events/ApplicationEvent.h"
#include "Pixel/Events/KeyEvent.h"
#include "Pixel/Events/MouseEvent.h"

namespace Pixel {

#ifndef PX_OPENGL
	class DescriptorHeap;
	class ContextManager;
	class Device;
	class GpuResource;
	class PixelBuffer;
	class DescriptorCpuHandle;
#endif

	class PIXEL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		ImGuiLayer(Ref<Device> pDevice);
		~ImGuiLayer();
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& e);
		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }
		void SetDarkThemeColors();
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;

#ifndef PX_OPENGL
		Ref<DescriptorHeap> m_srvHeap;
		//TODO:need to fix
		//ID3D12DescriptorHeap* m_SrvHeap;
		Ref<ContextManager> m_contextManager;
		Ref<Device> m_pDevice;

		Ref<PixelBuffer> m_BackBuffer[2];
		Ref<GpuResource> m_pGpuResource[2];
		//Ref<Context> m_DrawContext;
#endif
	};
}
