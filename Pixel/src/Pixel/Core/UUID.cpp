#include "pxpch.h"
#include "UUID.h"

//------cpp library------
#include <random>
#include <unordered_map>
//------cpp library------

namespace Pixel {

	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	UUID::UUID()
	:m_UUID(s_UniformDistribution(s_Engine))
	{
		
	}

	UUID::UUID(uint64_t uuid)
	:m_UUID(uuid)
	{

	}

	REFLECT_STRUCT_BEGIN(UUID)
	REFLECT_STRUCT_MEMBER(m_UUID)
	REFLECT_STRUCT_END()

}
