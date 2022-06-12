#pragma once

#include <memory>

namespace Pixel {
	class Device : public std::enable_shared_from_this<Device>
	{
	public:
		virtual ~Device();

		static Ref<Device> Get();

		static Ref<Device> m_pDevice;
	};
}
