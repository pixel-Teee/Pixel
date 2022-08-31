#include "pxpch.h"

#include "PutNode.h"

namespace Pixel {

	RTTR_REGISTRATION
	{
		using namespace rttr;
		registration::class_<PutNode>("PutNode")
			.constructor<PutNode::ValueType, std::string&>()
			.property("m_ValueType", &PutNode::m_ValueType)
			.property("m_NodeName", &PutNode::m_NodeName)
			.property("m_id", &PutNode::m_id);
	}
}