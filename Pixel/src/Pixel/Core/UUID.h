#pragma once

#include <xhash>

namespace Pixel {

	class Meta(Enable) UUID
	{
	public:
		//UUID id
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;
		~UUID() {};

		operator uint64_t() const { return m_UUID; }
	private:
		Meta()
		uint64_t m_UUID;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
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
