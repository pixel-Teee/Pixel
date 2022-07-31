#pragma once

#include <xhash>

namespace Pixel {

	class UUID
	{
	public:
		//UUID id
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};
}

namespace std {
	//provide hash function

	template<>
	struct hash<Pixel::UUID>
	{
		std::size_t operator()(const Pixel::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}
