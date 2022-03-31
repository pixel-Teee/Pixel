#include "pxpch.h"
#include "PutNode.h"

namespace Pixel {

	void InputNode::Connection(Ref<OutputNode> outputNode)
	{
		m_pOutputLink = outputNode;
	}

	void OutputNode::Connect(Ref<InputNode> inputNode)
	{
		m_pInputLink.push_back(inputNode);
	}

}