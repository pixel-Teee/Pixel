#include <Pixel.h>

class SandBox : public Pixel::Application
{
public:
	SandBox()
	{

	}

	~SandBox()
	{

	}
};

Pixel::Application* Pixel::CreateApplication()
{
	return new SandBox();
}