#include "pxpch.h"

#include "GraphLink.h"

namespace Pixel {
	bool operator==(const GraphLink& lhs, const GraphLink& rhs)
	{
		return lhs.m_LinkId == rhs.m_LinkId;
	}
	bool operator!=(const GraphLink& lhs, const GraphLink& rhs)
	{
		return lhs.m_LinkId != rhs.m_LinkId;
	}
}