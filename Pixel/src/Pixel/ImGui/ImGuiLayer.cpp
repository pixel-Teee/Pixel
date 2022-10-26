#include "pxpch.h"

//------other libary------
#include "imgui.h"
#include "ImGuizmo.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "backends/imgui_impl_dx12.h"
#include "backends/imgui_impl_glfw.h"
//------other libary------

#include "ImGuiLayer.h"

#include "Pixel/Core/Application.h"

//------descriptor related------
#include "Pixel/Renderer/DescriptorHandle/DescriptorGpuHandle.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorHeap.h"
#include "Pixel/Renderer/Descriptor/HeapType.h"
//------descriptor related------

//------buffer related------
#include "Pixel/Renderer/Buffer/GpuResource.h"
#include "Pixel/Renderer/Texture.h"
#include "Platform/DirectX/Buffer/DirectXColorBuffer.h"
#include "Platform/DirectX/Texture/DirectXTexture.h"
//------buffer related------

//------global object related------
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/DirectXSwapChain.h"
//------global object related------

//------context related------
#include "Pixel/Renderer/Context/ContextManager.h"
#include "Platform/DirectX/Context/DirectXContext.h"
#include "Platform/DirectX/Context/GraphicsContext.h"
//------context related------
#include "Platform/DirectX/d3dx12.h"

//------pipeline state object related------
#include "Pixel/Renderer/PipelineStateObject/RootSignature.h"
#include "Pixel/Renderer/PipelineStateObject/RootParameter.h"
//------pipeline state object related------

namespace Pixel {
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::ImGuiLayer(Ref<Device> pDevice)
	{
		
	}

	ImGuiLayer::~ImGuiLayer()
	{
		
	}

	void ImGuiLayer::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		float fontSize = 18.0f;// *2.0f;
		io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetDarkThemeColors();
		//SetPurpleThemeColors();

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		m_srvHeap = DescriptorHeap::Create(L"ImGuiLayerSrvHeap", DescriptorHeapType::CBV_UAV_SRV, 1024);
		
		std::static_pointer_cast<DirectXDescriptorHeap>(m_srvHeap)->GetComPtrHeap()->SetName(L"Imgui Descriptor Heap");

		m_srvHeap->Alloc(1);
		ImGui_ImplGlfw_InitForOther(window, true);
		ImGui_ImplDX12_Init(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice().Get(), std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->GetSwapChainBufferCount(),
			std::static_pointer_cast<DirectXDevice>(Device::Get())->GetBackBufferFormat(),
			std::static_pointer_cast<DirectXDescriptorHeap>(m_srvHeap)->GetHeapPointer(),
			std::static_pointer_cast<DirectXDescriptorHeap>(m_srvHeap)->GetHeapPointer()->GetCPUDescriptorHandleForHeapStart(),
			std::static_pointer_cast<DirectXDescriptorHeap>(m_srvHeap)->GetHeapPointer()->GetGPUDescriptorHandleForHeapStart()
		);

		//------Create Back Buffer-------
		m_BackBuffer[0] = GpuResource::CreateColorBuffer();
		m_BackBuffer[1] = GpuResource::CreateColorBuffer();
		//------Create Back Buffer-------

		//-------Create Back Buffer Resource------
		std::static_pointer_cast<DirectXColorBuffer>(m_BackBuffer[0])->CreateFromSwapChain(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->GetBackBufferSource(0), L"BackBuffer[0]");
		std::static_pointer_cast<DirectXColorBuffer>(m_BackBuffer[1])->CreateFromSwapChain(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->GetBackBufferSource(1), L"BackBuffer[1]");
		//-------Create Back Buffer Resource------
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiRender()
	{

	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();

		Ref<Context> EndContext = Device::Get()->GetContextManager()->CreateGraphicsContext(L"ImGuiLayer");

		uint32_t index = std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->GetCurrentBackBufferIndex();

		EndContext->TransitionResource(*m_BackBuffer[index], ResourceStates::RenderTarget);

		std::vector<Ref<DescriptorCpuHandle>> handles;
		Ref<DescriptorCpuHandle> rtvHandle = DescriptorCpuHandle::Create();
		rtvHandle->SetCpuHandle(&(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->GetRtvHandle(index)));
		handles.push_back(rtvHandle);

		//output merge stage
		EndContext->SetRenderTargets(1, handles);

		EndContext->FlushResourceBarriers();

		EndContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_srvHeap);

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), (ID3D12GraphicsCommandList*)(EndContext->GetNativeCommandList()));
		
		EndContext->TransitionResource(*m_BackBuffer[index], ResourceStates::Present, true);

		//dx present
		std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->Present();

		EndContext->Finish(true);
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}

	void ImGuiLayer::SetPurpleThemeColors()
	{
		//auto& colors = ImGui::GetStyle().Colors;
		//colors[ImGuiCol_WindowBg] = ImVec4{ 0.188f, 0.247f, 0.407f, 1.0f };

		//// Headers
		//colors[ImGuiCol_Header] = ImVec4{ 0.36f, 0.788f, 0.95f, 1.0f };
		//colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		//colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		//// Buttons
		//colors[ImGuiCol_Button] = ImVec4{ 0.36f, 0.788f, 0.95f, 1.0f };
		//colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		//colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		//// Frame BG
		//colors[ImGuiCol_FrameBg] = ImVec4{ 0.36f, 0.788f, 0.95f, 1.0f };
		//colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		//colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		//// Tabs
		//colors[ImGuiCol_Tab] = ImVec4{ 0.36f, 0.788f, 0.95f, 1.0f };
		//colors[ImGuiCol_TabHovered] = ImVec4{ 0.36f, 0.788f, 0.95f, 1.0f };
		//colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		//colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		//colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		//// Title
		//colors[ImGuiCol_TitleBg] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
		//colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		//colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}

	void ImGuiLayer::ResetSwapChain()
	{
		std::static_pointer_cast<DirectXGpuResource>(m_BackBuffer[0])->Destroy();
		std::static_pointer_cast<DirectXGpuResource>(m_BackBuffer[1])->Destroy();
	}

	void ImGuiLayer::ReCreateSwapChain()
	{
		m_BackBuffer[0] = GpuResource::CreateColorBuffer();
		m_BackBuffer[1] = GpuResource::CreateColorBuffer();

		std::static_pointer_cast<DirectXColorBuffer>(m_BackBuffer[0])->CreateFromSwapChain(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->GetBackBufferSource(0), L"BackBuffer[0]");
		std::static_pointer_cast<DirectXColorBuffer>(m_BackBuffer[1])->CreateFromSwapChain(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->GetBackBufferSource(1), L"BackBuffer[1]");
	}

	Ref<GpuResource> ImGuiLayer::GetBackBuffer()
	{
		uint32_t index = std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->GetCurrentBackBufferIndex();
		return m_BackBuffer[index];
	}

	Ref<DescriptorHeap> ImGuiLayer::GetSrvHeap()
	{
		return m_srvHeap;
	}

}



