#include "pxpch.h"

#include "InputNode.h"

namespace Pixel
{
	void InputNode::Connection(Ref<OutputNode> outputNode)
	{
		m_pOutLink = outputNode;
	}

	void InputNode::DisConnection()
	{
		m_pOutLink.reset();
	}

	Ref<OutputNode> InputNode::GetOutputLink() const
	{
		if (m_pOutLink.expired())
			return nullptr;
		return m_pOutLink.lock();
	}

	Ref<ShaderFunction> InputNode::GetOwner() const
	{
		if (m_pOwner.expired())
			return nullptr;
		return m_pOwner.lock();
	}
}
