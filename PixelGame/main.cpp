
#include "Application.h"

namespace Pixel {
	class PixelEditor : public Application
	{
	public:
		PixelEditor() :Application("Pixel Game")
		{
			
		}

		~PixelEditor()
		{

		}
	};

	Application* CreateApplication()
	{
		return new PixelEditor();
	}
}