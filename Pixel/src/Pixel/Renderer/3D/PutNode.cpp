#include "pxpch.h"
#include "PutNode.h"

namespace Pixel {

	void InputNode::Connection(Ref<OutputNode> outputNode)
	{
		m_pOutputLink = outputNode;
	}

	void InputNode::DisConnection()
	{
		m_pOutputLink = nullptr;
	}

	void OutputNode::Connect(Ref<InputNode> inputNode)
	{
		m_pInputLink.push_back(inputNode);
	}

}