#include <Pixel/Renderer/UniformBuffer.h>

namespace Pixel {
	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t Offset, uint32_t BufferSize, uint32_t BindIndex);

		virtual void SetData(uint32_t offset, uint32_t size, void* data) override;

		virtual void Bind();

	private:
		uint32_t m_RendererId;
		uint32_t m_BufferSize;
		uint32_t m_BindIndex;
		uint32_t m_Offset;
	};
}