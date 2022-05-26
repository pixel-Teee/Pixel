#pragma once

#include "DirectXContext.h"

namespace Pixel {
	class GraphicsContext : public DirectXContext
	{
	public:

		static GraphicsContext& Begin(const std::wstring& ID = L"");
	};
}
