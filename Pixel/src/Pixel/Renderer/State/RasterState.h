#pragma once

namespace Pixel {
	enum class CullMode
	{
		None,
		Back,
		Front
	};

	class RasterState
	{
	public:
		virtual ~RasterState();
		virtual void SetCullMode(CullMode cullMode) = 0;
		static Ref<RasterState> Create();
	};
}
