#include "pxpch.h"

#include "DirectXFrameBuffer.h"

#include "Platform/DirectX/Buffer/DirectXColorBuffer.h"
#include "Platform/DirectX/Buffer/DepthBuffer.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/Context/DirectXContextManager.h"
#include "Platform/DirectX/Context/DirectXContext.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"

namespace Pixel {

	static const uint32_t s_MaxFramebufferSize = 8192;

	namespace Utils {
		static bool DirectXIsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8: return true;
			}
			return false;
		}

		ImageFormat FrameBufferTextureFormatToImageFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
			case FramebufferTextureFormat::RGBA8:
				return ImageFormat::PX_FORMAT_R8G8B8A8_UNORM;
			case FramebufferTextureFormat::RGBA16F:
				return ImageFormat::PX_FORMAT_R16G16B16A16_FLOAT;
			case FramebufferTextureFormat::RED_INTEGER:
				return ImageFormat::PX_FORMAT_R32_SINT;
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				return ImageFormat::PX_FORMAT_D24_UNORM_S8_UINT;
			}
			PX_CORE_ASSERT(nullptr, "not find property image format!");
		}
	}

	DirectXFrameBuffer::DirectXFrameBuffer(const FramebufferSpecification& spec)
		:m_Specification(spec)
	{
		for (auto format : m_Specification.Attachments.Attachments)
		{
			if (!Utils::DirectXIsDepthFormat(format.TextureFormat))
			{
				m_ColorAttachmentSpecifications.emplace_back(format.TextureFormat);
			}
			else
				m_DepthAttachmentSpecification = format.TextureFormat;
		}

		Invalidate();
	}

	DirectXFrameBuffer::~DirectXFrameBuffer()
	{

	}

	void DirectXFrameBuffer::Invalidate()
	{
		//reset original's texture
		for (uint32_t i = 0; i < m_pColorBuffers.size(); ++i)
		{
			if (m_pColorBuffers[i] != nullptr && m_pColorBuffers[i]->GetResource() != nullptr)
			{
				m_pColorBuffers[i]->GetComPtrResource().Reset();
			}
		}

		if (m_pDepthBuffer != nullptr && m_pDepthBuffer->GetResource() != nullptr)
		{
			m_pDepthBuffer->GetComPtrResource().Reset();
		}

		if (m_ColorAttachmentSpecifications.size())
		{
			m_pColorBuffers.resize(m_ColorAttachmentSpecifications.size());

			uint32_t i = 0;
			std::wstring ColorBufferName;
			for (size_t i = 0; i < m_pColorBuffers.size(); ++i)
			{
				if (m_ColorAttachmentSpecifications[i].TextureFormat == FramebufferTextureFormat::RED_INTEGER)
					m_pColorBuffers[i] = CreateRef<DirectXColorBuffer>(glm::vec4(-1, -1, -1, -1));
				else
					m_pColorBuffers[i] = CreateRef<DirectXColorBuffer>(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
				ColorBufferName = L"RenderTarget" + std::to_wstring(i);
				//TODO:mipmaps
				m_pColorBuffers[i]->Create(ColorBufferName.c_str(), m_Specification.Width, m_Specification.Height, 0, Utils::FrameBufferTextureFormatToImageFormat(m_ColorAttachmentSpecifications[i].TextureFormat),
					nullptr);
			}
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			m_pDepthBuffer = CreateRef<DepthBuffer>();
			m_pDepthBuffer->Create(L"RenderTarget's Depth And Stencil", m_Specification.Width, m_Specification.Height, 1, Utils::FrameBufferTextureFormatToImageFormat(m_DepthAttachmentSpecification.TextureFormat), nullptr);
		}
	}

	void DirectXFrameBuffer::Bind()
	{
		Ref<Context> pContext = DirectXDevice::Get()->GetContextManager()->AllocateContext(CommandListType::Graphics);

		std::vector<Ref<DescriptorCpuHandle>> pHandles;
		for (uint32_t i = 0; i < m_pColorBuffers.size(); ++i)
		{
			pHandles.push_back(m_pColorBuffers[i]->GetRTV());
		}

		Ref<DescriptorCpuHandle> pDepthHandles;
		pDepthHandles = m_pDepthBuffer->GetDSV();

		pContext->SetRenderTargets(pHandles.size(), pHandles, pDepthHandles);

		pContext->Finish(true);
	}

	void DirectXFrameBuffer::UnBind()
	{

	}

	void DirectXFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width >= s_MaxFramebufferSize || height >= s_MaxFramebufferSize)
		{
			PIXEL_CORE_WARN("Attemped to resize framebuffer to {0}, {1}", width, height);
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}

	int DirectXFrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		//need to create read back buffer?
		return 0;
	}

	void DirectXFrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{

	}

	uint32_t DirectXFrameBuffer::GetRenderId()
	{
		//None
		return 0;
	}

	uint64_t DirectXFrameBuffer::GetColorAttachmentRendererID(uint32_t index /*= 0*/) const
	{
		return 0;
	}

	uint32_t DirectXFrameBuffer::GetDepthAttachmentRendererID() const
	{
		return 0;
	}

	void DirectXFrameBuffer::SetDepthAttachmentRendererID(uint32_t rendererID)
	{

	}

	const Pixel::FramebufferSpecification& DirectXFrameBuffer::GetSpecification() const
	{
		return m_Specification;
	}

	void DirectXFrameBuffer::CloseColorAttachmentDraw()
	{
		//None
	}

	void DirectXFrameBuffer::SetColorAttachmentDraw(uint32_t index)
	{
		//None
	}

	Ref<DescriptorCpuHandle> DirectXFrameBuffer::GetColorAttachmentDescriptorCpuHandle(uint32_t index)
	{
		return std::static_pointer_cast<DirectXColorBuffer>(m_pColorBuffers[index])->GetSRV();
	}

}

