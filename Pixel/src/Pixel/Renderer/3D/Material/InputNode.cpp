#include "pxpch.h"

#include "InputNode.h"
#include "OutputNode.h"

namespace Pixel
{
	void InputNode::Connection(Ref<OutputNode> outputNode)
	{
		m_pOutLink = outputNode;

		outputNode->m_pInputLinks.push_back(std::static_pointer_cast<InputNode>(shared_from_this()));
	}

	void InputNode::DisConnection()
	{
		Ref<OutputNode> pOutputNode = m_pOutLink.lock();

		std::vector<std::weak_ptr<InputNode>>::iterator it;

		for (auto& iter = pOutputNode->m_pInputLinks.begin(); iter != pOutputNode->m_pInputLinks.end(); ++iter)
		{
			if (iter->lock() == shared_from_this())
			{
				it = iter;
			}
		}

		//auto& iter = std::find(pOutputNode->m_pInputLinks.begin(), pOutputNode->m_pInputLinks.end(), std::static_pointer_cast<InputNode>(shared_from_this()));

		pOutputNode->m_pInputLinks.erase(it);

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
