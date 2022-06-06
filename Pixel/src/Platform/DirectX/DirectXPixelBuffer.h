#pragma once

#include "Pixel/Renderer/PixelBuffer.h"
#include "DirectXGpuResource.h"

namespace Pixel {
	class DirectXPixelBuffer : public PixelBuffer
	{
	public:
		DirectXPixelBuffer();

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }
		uint32_t GetDepth() const { return m_ArraySize; }
		const DXGI_FORMAT& GetFormat() const { return m_Format; }

		//has no effect on desktop
		void SetBankRotation(uint32_t RotationAmount);

		//write the raw pixel buffer contents to a file
		//the data is preceded by a 16-byte header: {DXGI_FORMAT, Pitch(in pixels), Width(in pixels), Height}
		void ExportToFile(const std::wstring& FilePath);

		//-------Pixel Engine Format To DXGIFormat------
		DXGI_FORMAT FormatToDXGIFormat(ImageFormat Format);
		ImageFormat DXGIFormatToImageFormat(DXGI_FORMAT dxgiFormat);

		virtual void CreateFromSwapChain(const std::wstring& Name, GpuResource* pBaseResource) override;

		//-------Pixel Engine Format To DXGIFormat------
	protected:

		D3D12_RESOURCE_DESC DescribeTex2D(uint32_t Width, uint32_t Height, uint32_t DepthOrArraySize, uint32_t NumMips, DXGI_FORMAT Format,
			UINT Flags);

		void AssociateWithResource(const std::wstring& Name, ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState);

		void CreateTextureResource(const std::wstring& Name, const D3D12_RESOURCE_DESC& ResourceDesc,
			D3D12_CLEAR_VALUE ClearValue, Ref<GpuVirtualAddress> VideoMemoryPtr);

		/*
		void CreateTextureResource(const std::wstring& Name, const D3D12_RESOURCE_DESC& ResourceDesc,
			D3D12_CLEAR_VALUE ClearValue, EsramAllocator* Allocator);
		*/

		static DXGI_FORMAT GetBaseFormat(DXGI_FORMAT Format);
		static DXGI_FORMAT GetUAVFormat(DXGI_FORMAT Format);
		static DXGI_FORMAT GetDSVFormat(DXGI_FORMAT Format);
		static DXGI_FORMAT GetDepthFormat(DXGI_FORMAT Format);
		static DXGI_FORMAT GetStencilFormat(DXGI_FORMAT Format);
		//per pixel's bytes
		static size_t BytesPerPixel(DXGI_FORMAT Format);

		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_ArraySize;
		DXGI_FORMAT m_Format;
		uint32_t m_BankRotation;//??

		DirectXGpuResource m_GpuResource;
	};
}
