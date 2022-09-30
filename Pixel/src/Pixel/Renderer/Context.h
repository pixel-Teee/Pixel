#pragma once

namespace Pixel {

	class Context
	{
	public:
		virtual void Initialize() = 0;

		//TODO:refractor this
		virtual void SwapBuffers() = 0;

		virtual void Reset() = 0;

		virtual ~Context();
	};

}
