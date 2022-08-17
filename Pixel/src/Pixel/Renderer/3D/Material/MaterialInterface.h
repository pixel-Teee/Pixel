#pragma once

namespace Pixel {
	class MaterialInterface {
	public:
		virtual ~MaterialInterface() = default;

		static void Create();
	};
}