#include <Pixel.h>

class ExampleLayer : public Pixel::Layer
{
public:
	ExampleLayer():Layer("Example"){}

	void OnUpdate()override
	{
		if(Pixel::Input::IsKeyPressed(PX_KEY_TAB))
			PIXEL_CORE_TRACE("Tab Key is Pressed!");
	}

	void OnEvent(Pixel::Event& event)override
	{
		//PIXEL_TRACE("{0}", event);
	}
};

class SandBox : public Pixel::Application
{
public:
	SandBox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Pixel::ImGuiLayer());
	}

	~SandBox()
	{

	}
};

Pixel::Application* Pixel::CreateApplication()
{
	return new SandBox();
}