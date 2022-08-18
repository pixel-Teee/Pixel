#pragma once

namespace Pixel {
	class PutNode {
	public:
		//shader variable type
		//float float2 float3 float4
		enum class ValueType
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
			m_ValueType = valueType;

			m_NodeName = nodeName;
		}

		//------about value type------
		ValueType GetValueType()
		{
			return m_ValueType;
		}

		void SetValueType(ValueType valueType)
		{
			m_ValueType = valueType;
		}
		//------about value type------

		//------about name------
		std::string GetNodeName()
		{
			return m_NodeName;
		}

		void SetNodeName(const std::string& nodeName)
		{
			m_NodeName = nodeName;
		}
		//------about name------

		//node pin value type
		ValueType m_ValueType;

		//node pin name
		std::string m_NodeName;
	};
}
