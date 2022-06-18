#include "pxpch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <Glad/glad.h>

#include "Pixel/Renderer/Buffer/GpuResource.h"
#include "Pixel/Renderer/VertexBufferView.h"
#include "Pixel/Renderer/IndexBufferView.h"

namespace Pixel {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle):m_WindowHandle(windowHandle)
	{
		PX_CORE_ASSERT(windowHandle, "Window Handle is NULL");
	}

	OpenGLContext::~OpenGLContext()
	{

	}

	void OpenGLContext::Initialize()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		//glad¼ÓÔØOpenGLµÄº¯Êý
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		PX_CORE_ASSERT(status, "Failed to initialize Glad!");

		PIXEL_CORE_INFO("OpenGL Info:");
		PIXEL_CORE_INFO("Vendor : {0}", glGetString(GL_VENDOR));
		PIXEL_CORE_INFO("Renderer : {0}", glGetString(GL_RENDERER));
		PIXEL_CORE_INFO("Version : {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

	void* OpenGLContext::GetNativeCommandList()
	{
		return (void*)(0);
	}

	void OpenGLContext::Reset()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	CommandListType OpenGLContext::GetType()
	{
		return CommandListType::Graphics;
	}

	void OpenGLContext::SetID(const std::wstring& ID)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::FlushResourceBarriers()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	uint64_t OpenGLContext::Flush(bool WaitForCompletion)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	uint64_t OpenGLContext::Finish(bool WaitForCompletion)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::CopyBuffer(GpuResource& Dest, GpuResource& Src)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::CopyBufferRegion(GpuResource& Dest, size_t DestOffset, GpuResource& Src, size_t SrcOffset, size_t NumBytes)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::TransitionResource(GpuResource& Resource, ResourceStates NewState, bool FlushImmediate /*= false*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::CopySubresource(GpuResource& Dest, uint32_t DestSubIndex, GpuResource& Src, uint32_t SrcSubIndex)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::InsertUAVBarrier(GpuResource& Resource, bool FlushImmediate)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::WriteBuffer(GpuResource& Dest, size_t DestOffset, const void* Data, size_t NumBytes)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetRootSignature(const RootSignature& RootSig)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetDescriptorHeap(DescriptorHeapType Type, Ref<DescriptorHeap> HeapPtr)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::ClearColor(GpuResource& Target, PixelRect* Rect)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::ClearColor(GpuResource& Target, float Color[4], PixelRect* Rect /*= nullptr*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::ClearDepth(GpuResource& Target)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::ClearStencil(GpuResource& Target)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::ClearDepthAndStencil(GpuResource& Target)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetRenderTargets(uint32_t NumRTVs, const std::vector<Ref<DescriptorCpuHandle>>& RTVs)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetRenderTargets(uint32_t NumRTVs, const std::vector<Ref<DescriptorCpuHandle>>& RTVs, const Ref<DescriptorCpuHandle>& DSV)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetRenderTarget(Ref<DescriptorCpuHandle> RTV)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetRenderTarget(Ref<DescriptorCpuHandle> RTV, Ref<DescriptorCpuHandle> DSV)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetDepthStencilTarget(Ref<DescriptorCpuHandle> DSV)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetViewport(const ViewPort& vp)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetViewport(float x, float y, float w, float h, float minDepth /*= 0.0f*/, float maxDepth /*= 1.0f*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetScissor(const PixelRect& rect)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetViewportAndScissor(const ViewPort& vp, const PixelRect& rect)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetStencilRef(uint32_t StencilRef)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetBlendFactor(glm::vec4 BlendFactor)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetPrimitiveTopology(PrimitiveTopology Topology)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetConstantArray(uint32_t RootIndex, uint32_t NumConstants, const void* pConstants)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetConstant(uint32_t RootIndex, uint32_t Offset, uint32_t Val)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetConstants(uint32_t RootIndex, uint32_t x)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetConstants(uint32_t RootIndex, uint32_t x, uint32_t y, uint32_t z, uint32_t w)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetConstantBuffer(uint32_t RootIndex, Ref<GpuVirtualAddress> CBV)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetDynamicConstantBufferView(uint32_t RootIndex, size_t BufferSize, const void* BufferData)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetBufferSRV(uint32_t RootIndex, const GpuResource& SRV, uint64_t Offset /*= 0*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetBufferUAV(uint32_t RootIndex, const GpuResource& UAV, uint64_t Offset /*= 0*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetDescriptorTable(uint32_t RootIndex, Ref<DescriptorGpuHandle> FirstHandle)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetIndexBuffer(const Ref<IBV> IBView)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetVertexBuffer(uint32_t Slot, const Ref<VBV> VBView)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetVertexBuffers(uint32_t StartSlot, uint32_t Count, const std::vector<Ref<VBV>> VBViews)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetDynamicVB(uint32_t Slot, size_t NumVertices, size_t VertexStride, const void* VBData)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetDynamicIB(size_t IndexCount, const uint64_t* IBData)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetDynamicSRV(uint32_t RootIndex, size_t BufferSize, const void* BufferData)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::Draw(uint32_t VertexCount, uint32_t VertexStartOffset /*= 0*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::DrawIndexed(uint32_t IndexCount, uint32_t StartIndexLocation /*= 0*/, int32_t BaseVertexLocation /*= 0*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::DrawInstanced(uint32_t VertexCountPerInstance, uint32_t InstanceCount, uint32_t StartVertexLocation /*= 0*/, uint32_t StartInstanceLocation /*= 0*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::DrawIndexedInstanced(uint32_t IndexCountPerInstance, uint32_t InstanceCount, uint32_t StatrIndexLocation, int32_t BaseVertexLocation, uint32_t StartInstanceLocation)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetPipelineState(const PSO& pso)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::Dispatch(size_t GroupCountX /*= 1*/, size_t GroupCountY /*= 1*/, size_t GroupCountZ /*= 1*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::Dispatch1D(size_t ThreadCountX, size_t GroupSizeX /*= 64*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::Dispatch2D(size_t ThreadCountX, size_t ThreadCountY, size_t GroupSizeX /*= 8*/, size_t GroupSizeY /*= 8*/)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void OpenGLContext::SetType(CommandListType type)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}