#pragma once

#include "Pixel/Core/Core.h"
#include <string>

namespace Pixel {
	class PutNode {
	public:
		//shader variable type
		//float vec2 vec3 vec4
		enum ValueType
		{
			VT_1,
			VT_2,
			VT_3,
			VT_4,
			VT_MAX
		};
		PutNode() = default;
		PutNode(ValueType valueType, std::string& nodeName)
		{
			m_valueType = valueType;
			m_nodeName = nodeName;
		}
		ValueType m_valueType;
		std::string m_nodeName;
		ValueType GetValueType()
		{
			return m_valueType;
		}
		void SetValueType(ValueType valueType)
		{
			m_valueType = valueType;
		}
		std::string GetNodeName()
		{
			return m_nodeName;
		}
		void SetNodeName(const std::string& name)
		{
			m_nodeName = name;
		}
	};

	class ShaderFunction;
	class OutputNode;
	class InputNode : public PutNode
	{
	public:
		//shader function
		Ref<ShaderFunction> m_pOwner;
		Ref<OutputNode> m_pOutputLink;

		InputNode() = default;
		InputNode(ValueType valueType, std::string& nodeName, Ref<ShaderFunction> pShaderFunction) : PutNode(valueType, nodeName)
		{
			m_pOwner = pShaderFunction;
		}

		void Connection(Ref<OutputNode> outputNode);
		void DisConnection();

		const Ref<OutputNode> GetOutputLink() const
		{
			return m_pOutputLink;
		}

		//get shader function
		Ref<ShaderFunction> GetOwner() const
		{
			return m_pOwner;
		}
	};

	class OutputNode : public PutNode
	{
	public:
		//shader function
		Ref<ShaderFunction> m_pOwner;
		std::vector<Ref<InputNode>> m_pInputLink;

		OutputNode() = default;
		OutputNode(ValueType valueType, std::string& nodeName, Ref<ShaderFunction> pShaderFunction) : PutNode(valueType, nodeName)
		{
			m_pOwner = pShaderFunction;
		}

		const std::vector<Ref<InputNode>>& GetInputLink() const
		{
			return m_pInputLink;
		}

		//get shader function
		Ref<ShaderFunction> GetOwner() const
		{
			return m_pOwner;
		}

		void Connect(Ref<InputNode> inputNode);
	};
}
