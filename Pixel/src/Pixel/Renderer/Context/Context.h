#pragma once

namespace Pixel {

	class GpuResource;

	class Context
	{
	public:
		virtual ~Context();

		virtual void Initialize() = 0;

		virtual void Reset() = 0;

		//TODO:refractor this
		virtual void SwapBuffers() = 0;
		
		virtual void SetID(const std::wstring& ID) = 0;

		virtual void FlushResourceBarriers() = 0;

		//flush existing commands to the gpu but keep the context alive
		virtual uint64_t Flush(bool WaitForCompletion) = 0;

		//flush existing commands and release the current context
		virtual uint64_t Finish(bool WaitForCompletion) = 0;

		//------Buffer Operation------
		virtual void CopyBuffer(GpuResource& Dest, GpuResource& Src) = 0;
		virtual void CopyBufferRegion(GpuResource& Dest, size_t DestOffset, GpuResource& Src, size_t SrcOffset, size_t NumBytes) = 0;

		//texture copy, 1D texture copy
		virtual void CopySubresource(GpuResource& Dest, uint32_t DestSubIndex, GpuResource& Src, uint32_t SrcSubIndex) = 0;
		
		virtual void InsertUAVBarrier(GpuResource& Resource, bool FlushImmediate) = 0;
		//------Buffer Operation------

		virtual void WriteBuffer(GpuResource& Dest, size_t DestOffset, const void* Data, size_t NumBytes) = 0;
	};

}
