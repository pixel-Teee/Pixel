#pragma once

#include "PutNode.h"

namespace Pixel
{
	class ShaderFunction;

	class InputNode;

	class OutputNode : public PutNode
	{
	public:
		//shader function
		std::weak_ptr<ShaderFunction> m_pOwner;

		std::vector<std::weak_ptr<InputNode>> m_pInputLinks;

		OutputNode() = default;

		OutputNode(ValueType valueType, std::string& nodeName, Ref<ShaderFunction> pShaderFunction) : PutNode(valueType, nodeName)
		{
			m_pOwner = pShaderFunction;
		}

		std::vector<Ref<InputNode>>& GetInputLink() const;

		//get shader function
		Ref<ShaderFunction> GetOwner() const;

		friend class InputNode;
	};
}
