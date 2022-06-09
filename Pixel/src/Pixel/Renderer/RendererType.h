#pragma once

namespace Pixel {

	struct PixelRect {
		int32_t Left;
		int32_t Right;
		int32_t Top;
		int32_t bottom;
	};

	struct ViewPort
	{
		float TopLeftX;
		float TopLeftY;
		float Width;
		float Height;
		float MinDepth;
		float MaxDepth;
	};

	enum PrimitiveTopology
	{
		UNDEFINED,
		LINELIST,
		LINESTRIP,
		TRIANGLELIST,
		TRIANGLESTRIP
	};
}