#pragma once

//------my library------
#include "Pixel/Core/Layer.h"
#include "Pixel/Events/ApplicationEvent.h"
#include "Pixel/Events/KeyEvent.h"
#include "Pixel/Events/MouseEvent.h"
//------my library------

namespace Pixel {
	//------forward declare------
	class DescriptorCpuHandle;
	class DescriptorHeap;

	class Device;
	class ContextManager;
	
	class GpuResource;
	class PixelBuffer;
	
	class Texture2D;
	//------forward declare------

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
		//------theme related------
		void SetDarkThemeColors();
		void SetPurpleThemeColors();
		//------theme related------

		//------swap chain related------
		void ResetSwapChain();
		void ReCreateSwapChain();
		Ref<GpuResource> GetBackBuffer();
		//------swap chain related------
		
		Ref<DescriptorHeap> GetSrvHeap();
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;

		//------image related------
		Ref<DescriptorHeap> m_srvHeap;
		Ref<DescriptorCpuHandle> m_imageHandle;
		//------image related------

		Ref<GpuResource> m_BackBuffer[2];
	};
}
