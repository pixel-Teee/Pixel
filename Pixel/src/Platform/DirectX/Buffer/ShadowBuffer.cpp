#include "pxpch.h"
#include "ShadowBuffer.h"
#include "Platform/DirectX/Context/GraphicsContext.h"

namespace Pixel {


	ShadowBuffer::ShadowBuffer()
	{

	}

	void ShadowBuffer::Create(const std::wstring& Name, uint32_t Width, uint32_t Height, Ref<GpuVirtualAddress> VideoMemoryPtr)
	{
		DepthBuffer::Create(Name, Width, Height, ImageFormat::PX_FORMAT_D16_UNORM, VideoMemoryPtr);

		m_ViewPort.TopLeftX = 0.0f;
		m_ViewPort.TopLeftY = 0.0f;
		m_ViewPort.Width = (float)Width;
		m_ViewPort.Height = (float)Height;
		m_ViewPort.MinDepth = 0.0f;
		m_ViewPort.MaxDepth = 1.0f;

		m_Scissor.Left = 1;
		m_Scissor.Top = 1;
		m_Scissor.Right = Width - 2;
		m_Scissor.Bottom = Height - 2;
	}

	void ShadowBuffer::Create(const std::wstring& Name, uint32_t Width, uint32_t Height)
	{
		DepthBuffer::Create(Name, Width, Height, ImageFormat::PX_FORMAT_D16_UNORM, nullptr);

		m_ViewPort.TopLeftX = 0.0f;
		m_ViewPort.TopLeftY = 0.0f;
		m_ViewPort.Width = (float)Width;
		m_ViewPort.Height = (float)Height;
		m_ViewPort.MinDepth = 0.0f;
		m_ViewPort.MaxDepth = 1.0f;

		m_Scissor.Left = 1;
		m_Scissor.Top = 1;
		m_Scissor.Right = Width - 2;
		m_Scissor.Bottom = Height - 2;
	}

	void ShadowBuffer::BeginRendering(GraphicsContext& context)
	{
		context.TransitionResource(*this, ResourceStates::DepthWrite, true);
		context.ClearDepth(*this);
		context.SetDepthStencilTarget(GetDSV());
		context.SetViewportAndScissor(m_ViewPort, m_Scissor);
	}

	void ShadowBuffer::EndRendering(GraphicsContext& context)
	{
		//srv
		context.TransitionResource(*this, ResourceStates::PixelShaderResource);
	}

}