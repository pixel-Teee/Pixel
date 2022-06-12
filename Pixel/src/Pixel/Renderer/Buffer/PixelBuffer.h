#pragma once

#include "glm/glm.hpp"

namespace Pixel {

	class GpuResource;
	class Device;
	class PixelBuffer
	{
	public:

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetDepth() const = 0;

		virtual void SetGpuResource(Ref<GpuResource> pResource) = 0;

		virtual void CreateFromSwapChain(const std::wstring& Name, Ref<Device> pDevice) = 0;
		
		virtual void SetBankRotation(uint32_t RotationAmount) = 0;
		virtual void ExportToFile(const std::wstring& FilePath) = 0;

		static Ref<PixelBuffer> CreatePixelBuffer();
		static Ref<PixelBuffer> CreateColorBuffer(const glm::vec4& ClearColor);
		//static Ref<PixelBuffer> CreateDepthBuffer();
	};
}