#pragma once

namespace Pixel {
	enum class Meta(Enable) ValueType
	{
		VT_1,
		VT_2,
		VT_3,
		VT_4,
		VT_4X4,//float4x4
		VT_MAX
	};

	class Meta(Enable) PutNode : public std::enable_shared_from_this<PutNode> {
	public:
		//shader variable type
		//float float2 float3 float4


		Meta()
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

		void SetPutNodeId(uint32_t pinId) { m_id = pinId; }

		uint32_t GetPutNodeId() { return m_id; }

		Meta()
		//node pin value type
		ValueType m_ValueType;
		Meta()
		//node pin name
		std::string m_NodeName;
		Meta()
		//node pin id
		uint32_t m_id;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}
