#include <Pixel.h>

class ExampleLayer : public Pixel::Layer
{
public:
	ExampleLayer():Layer("Example"){}

	void OnUpdate()override
	{
		PIXEL_INFO("ExampleLayer::Update");
	}

	void OnEvent(Pixel::Event& event)override
	{
		PIXEL_TRACE("{0}", event);
	}
};

class SandBox : public Pixel::Application
{
public:
	SandBox()
	{
		PushLayer(new ExampleLayer());
	}

	~SandBox()
	{

	}
};

Pixel::Application* Pixel::CreateApplication()
{
	return new SandBox();
}