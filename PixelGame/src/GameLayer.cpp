#include "GameLayer.h"
#include "Pixel/Scene/SceneSerializer.h"

namespace Pixel {
	extern const std::filesystem::path g_AssetPath;

	void GameLayer::OnAttach()
	{
		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA8,
		FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 970;
		m_GeoFramebuffer = Framebuffer::Create(fbSpec);

		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		//m_GameScene = CreateRef<Scene>();

		OpenScene();
		m_GameScene->OnRuntimeStart();
		//m_ViewPortSize = { 1280, 970 };
	}

	void GameLayer::OnDetach()
	{
		
	}

	void GameLayer::OnUpdate(Timestep ts)
	{
		//Resize
		FramebufferSpecification spec = m_Framebuffer->GetSpecification();

		if (spec.Width != m_ViewPortSize.x && spec.Height != m_ViewPortSize.y)
		{
			//RenderCommand::SetViewport(0, 0, m_ViewPortSize.x, m_ViewPortSize.y);
			m_Framebuffer->Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			m_GeoFramebuffer->Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			m_GameScene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		}
		
		m_GameScene->OnUpdateRuntime(ts, m_GeoFramebuffer, m_Framebuffer);

		BindReadFramebuffer(m_Framebuffer->GetRenderId());
		BindWriteFramebuffer(0);
		BlitFramebuffer(m_ViewPortSize.x, m_ViewPortSize.y);
	}

	void GameLayer::OnEvent(Event& event)
	{
		EventDispatcher dispather(event);
		dispather.Dispatch<WindowResizeEvent>(PX_BIND_EVENT_FN(GameLayer::OnWindowResizeEvent));
	}

	void GameLayer::OpenScene()
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		newScene->SetViewPortSize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		SceneSerializer serializer(newScene);
		//TODO:need to fix
		newScene->SetSkyBox(Renderer3D::GetSkyBox());
		if (serializer.Deserialize("assets//scenes//Test4.pixel"))
		{
			m_GameScene = newScene;

			m_GameScene->OnViewportResize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		}
	}

	bool GameLayer::OnWindowResizeEvent(WindowResizeEvent& e)
	{
		m_ViewPortSize = {e.GetWidth(), e.GetHeight()};
		return true;
	}
}