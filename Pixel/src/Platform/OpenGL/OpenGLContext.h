#pragma once

#include "Pixel/Renderer/Context/Context.h"

struct GLFWwindow;

namespace Pixel {

	class OpenGLContext : public Context
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);
		virtual ~OpenGLContext();
		virtual void Initialize() override;
		virtual void SwapBuffers() override;
		virtual void Reset() override;

		virtual void SetID(const std::wstring& ID) override;

		virtual void FlushResourceBarriers() override;

		virtual uint64_t Flush(bool WaitForCompletion) override;

		virtual uint64_t Finish(bool WaitForCompletion) override;

		virtual void CopyBuffer(GpuResource& Dest, GpuResource& Src) override;

		virtual void CopyBufferRegion(GpuResource& Dest, size_t DestOffset, GpuResource& Src, size_t SrcOffset, size_t NumBytes) override;

		virtual void CopySubresource(GpuResource& Dest, uint32_t DestSubIndex, GpuResource& Src, uint32_t SrcSubIndex) override;

		virtual void InsertUAVBarrier(GpuResource& Resource, bool FlushImmediate) override;

		virtual void WriteBuffer(GpuResource& Dest, size_t DestOffset, const void* Data, size_t NumBytes) override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}
