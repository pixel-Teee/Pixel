#pragma once

#include "PutNode.h"

namespace Pixel
{
	class ShaderFunction;

	class OutputNode;

	class InputNode : public PutNode
	{
	public:
		//shader function
		std::weak_ptr<ShaderFunction> m_pOwner;

		//input node connect to out put node
		std::weak_ptr<OutputNode> m_pOutLink;

		InputNode() = default;

		InputNode(ValueType valueType, std::string& nodeName, Ref<ShaderFunction> pShaderFunction) : PutNode(valueType, nodeName)
		{
			m_pOwner = pShaderFunction;//weak ptr construct from the shared_ptr
		}

		void Connection(Ref<OutputNode> outputNode);

		void DisConnection();

		Ref<OutputNode> GetOutputLink() const;

		//get shader function
		Ref<ShaderFunction> GetOwner() const;
	};
}
