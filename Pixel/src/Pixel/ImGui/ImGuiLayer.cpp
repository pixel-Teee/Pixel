#include "pxpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"

#ifdef PX_OPENGL
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#else
#include "backends/imgui_impl_dx12.h"
#include "backends/imgui_impl_glfw.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/DirectXSwapChain.h"
#include "Platform/DirectX/Descriptor/DirectXDescriptorHeap.h"
#include "Pixel/Renderer/Descriptor/HeapType.h"
#include "Pixel/Renderer/Context/ContextManager.h"
#include "Platform/DirectX/Context/DirectXContext.h"
#include "Platform/DirectX/DescriptorHandle/DirectXDescriptorCpuHandle.h"
#include "Platform/DirectX/d3dx12.h"
#include "Pixel/Renderer/Buffer/GpuResource.h"
#include "Platform/DirectX/Context/GraphicsContext.h"
#include "Pixel/Renderer/PipelineStateObject/RootSignature.h"
#include "Pixel/Renderer/PipelineStateObject/RootParameter.h"
#include "Platform/DirectX/DirectXDevice.h"
#include "Platform/DirectX/Buffer/DirectXColorBuffer.h"
#include "Pixel/Renderer/Texture.h"
#include "Platform/DirectX/Texture/DirectXTexture.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorGpuHandle.h"
#endif

#include "Pixel/Core/Application.h"

//TEMPORARAY
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "ImGuizmo.h"

namespace Pixel {
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::ImGuiLayer(Ref<Device> pDevice)
	{
		//m_pDevice = pDevice;
	}

	ImGuiLayer::~ImGuiLayer()
	{
		//m_contextManager->DestroyAllContexts();
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

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

#ifdef PX_OPENGL
		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
#else
		//m_contextManager = ContextManager::Create();

		m_srvHeap = DescriptorHeap::Create(L"ImGuiLayerSrvHeap", DescriptorHeapType::CBV_UAV_SRV, 2);
		//m_imageSrvHeap = DescriptorHeap::Create(L"ImageSrvHeap", DescriptorHeapType::CBV_UAV_SRV, 1);
		

		std::static_pointer_cast<DirectXDescriptorHeap>(m_srvHeap)->GetComPtrHeap()->SetName(L"Srv Descriptor Heap");

		m_srvHeap->Alloc(1);
		ImGui_ImplGlfw_InitForOther(window, true);
		ImGui_ImplDX12_Init(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetDevice().Get(), std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->GetSwapChainBufferCount(),
			std::static_pointer_cast<DirectXDevice>(Device::Get())->GetBackBufferFormat(),
			std::static_pointer_cast<DirectXDescriptorHeap>(m_srvHeap)->GetHeapPointer(),
			std::static_pointer_cast<DirectXDescriptorHeap>(m_srvHeap)->GetHeapPointer()->GetCPUDescriptorHandleForHeapStart(),
			std::static_pointer_cast<DirectXDescriptorHeap>(m_srvHeap)->GetHeapPointer()->GetGPUDescriptorHandleForHeapStart()
		);
		
		m_imageHandle = m_srvHeap->Alloc(1)->GetCpuHandle();
		
		////------Create Back Buffer-------
		m_BackBuffer[0] = GpuResource::CreateColorBuffer();
		m_BackBuffer[1] = GpuResource::CreateColorBuffer();
		////------Create Back Buffer-------

		////-------Create Back Buffer Resource------
		std::static_pointer_cast<DirectXColorBuffer>(m_BackBuffer[0])->CreateFromSwapChain(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->GetBackBufferSource(0), L"BackBuffer[0]");
		std::static_pointer_cast<DirectXColorBuffer>(m_BackBuffer[1])->CreateFromSwapChain(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->GetBackBufferSource(1), L"BackBuffer[1]");
		////-------Create Back Buffer Resource------

		m_pTexture = Texture2D::Create("assets//textures//Material_Albedo.dds");
		
		//copy the texture's handle to descriptor heap's handle, then bind the heap

		Device::Get()->CopyDescriptorsSimple(1, m_imageHandle, m_pTexture->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);;
		//Device::Get()->CopyDescriptorsSimple(1, std::static_pointer_cast<DirectXDescriptorHeap>(m_imageSrvHe)
#endif
	}

	void ImGuiLayer::OnDetach()
	{
		//ImGuiIO& io = ImGui::GetIO();
		//io.Fonts->Clear();
#ifdef PX_OPENGL
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
#else
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
#endif
	}

	void ImGuiLayer::OnImGuiRender()
	{
		ImGui::ShowDemoWindow();
		
		Ref<Context> pContext = Device::Get()->GetContextManager()->AllocateContext(CommandListType::Graphics);

		pContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_srvHeap);
		//pContext->SetDescriptorHeap()
		//Ref<Context> 
		ImGui::Begin("DirectX12 Texture Test");

		Ref<DescriptorGpuHandle> handle = (*m_srvHeap)[1].GetGpuHandle();
		//EndContext->TransitionResource(*(std::static_pointer_cast<DirectXTexture>(m_pTexture)->m_pGpuResource), ResourceStates::Present);
		ImGui::Image((ImTextureID)(handle->GetGpuPtr()), ImVec2(256, 256));
		ImGui::End();

		pContext->Finish(true);
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
#ifdef PX_OPENGL
		ImGui_ImplOpenGL3_NewFrame();
#else
		ImGui_ImplDX12_NewFrame();
#endif
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
#ifdef PX_OPENGL
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#else
		
		Ref<Context> EndContext = Device::Get()->GetContextManager()->CreateGraphicsContext(L"ImGuiLayer");

		uint32_t index = std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->GetCurrentBackBufferIndex();

		EndContext->TransitionResource(*m_BackBuffer[index], ResourceStates::RenderTarget);

		std::vector<Ref<DescriptorCpuHandle>> handles;
		Ref<DescriptorCpuHandle> rtvHandle = DescriptorCpuHandle::Create();
		rtvHandle->SetCpuHandle(&(std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->GetRtvHandle(index)));
		handles.push_back(rtvHandle);
		////output merge stage
		EndContext->SetRenderTargets(1, handles);
		//
		float color[4] = { 0.2f, 0.3f, 0.7f, 1.0f };

		EndContext->ClearColor(*m_BackBuffer[index].get(), color);

	//	ImGui::Begin("DirectX12 Texture Test");
	//	EndContext->TransitionResource(*(std::static_pointer_cast<DirectXTexture>(m_pTexture)->m_pGpuResource), ResourceStates::Present);
		//ImGui::Image((ImTextureID)(m_pTexture->GetRendererID()), ImVec2(256, 256));
	//	ImGui::End();
		EndContext->SetDescriptorHeap(DescriptorHeapType::CBV_UAV_SRV, m_srvHeap);

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), (ID3D12GraphicsCommandList*)(EndContext->GetNativeCommandList()));
		//
		EndContext->TransitionResource(*m_BackBuffer[index], ResourceStates::Present, true);

		////dx present
		std::static_pointer_cast<DirectXDevice>(Device::Get())->GetSwapChain()->Present();

		EndContext->Finish(true);
#endif
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

}



