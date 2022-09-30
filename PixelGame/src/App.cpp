#include <Pixel.h>
#include <Pixel/Core/EntryPoint.h>

#include "GameLayer.h"

namespace Pixel {
	class PixelGame : public Application
	{
	public:
		PixelGame() :Application("Pixel Game")
		{
			PushLayer(new GameLayer());
		}

		~PixelGame()
		{

		}
	};

	Application* CreateApplication()
	{
		return new PixelGame();
	}
}