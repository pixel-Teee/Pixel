#include "pxpch.h"

#include "OutputNode.h"

namespace Pixel
{
	std::vector<Ref<InputNode>>& OutputNode::GetInputLink() const
	{
		std::vector<Ref<InputNode>> result;
		for (size_t i = 0; i < m_pInputLinks.size(); ++i)
		{
			if (m_pInputLinks[i].expired())
				result.push_back(nullptr);
			else
				result.push_back(m_pInputLinks[i].lock());
		}
		return result;
	}

	Ref<ShaderFunction> OutputNode::GetOwner() const
	{
		if (m_pOwner.expired())
			return nullptr;
		return m_pOwner.lock();
	}
}
