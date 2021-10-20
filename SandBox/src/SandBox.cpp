#include <Pixel.h>

class ExampleLayer : public Pixel::Layer
{
public:
	ExampleLayer():Layer("Example"){
	
		auto cam = camera(5.0f, {2.0f, 1.0f});
	}

	void OnUpdate()override
	{
		if(Pixel::Input::IsKeyPressed(PX_KEY_TAB))
			PIXEL_CORE_TRACE("Tab Key is Pressed!");
	}

	void OnEvent(Pixel::Event& event)override
	{
		if (event.GetEventType() == Pixel::EventType::KeyPressed)
		{
			Pixel::KeyPressedEvent& KeyEvent = (Pixel::KeyPressedEvent&)event;
			PIXEL_TRACE("{0}", (char)KeyEvent.GetKeyCode());
		}
	}
};

class SandBox : public Pixel::Application
{
public:
	SandBox()
	{
		PushLayer(new ExampleLayer());
		//PushOverlay(new Pixel::ImGuiLayer());
	}

	~SandBox()
	{

	}
};

Pixel::Application* Pixel::CreateApplication()
{
	return new SandBox();
}