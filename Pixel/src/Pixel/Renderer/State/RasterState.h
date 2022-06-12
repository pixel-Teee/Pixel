#pragma once

namespace Pixel {
	class RasterState
	{
	public:
		virtual ~RasterState();
		static Ref<RasterState> Create();
	};
}
