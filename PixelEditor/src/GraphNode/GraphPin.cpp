#include "pxpch.h"

#include "GraphPin.h"

namespace Pixel {
	bool operator==(const GraphPin& lhs, const GraphPin& rhs)
	{
		return false;
	}
	GraphPin::GraphPin()
	{
		m_Color = ImColor(255, 255, 255, 255);
	}
}