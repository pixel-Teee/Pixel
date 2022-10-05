#include "pxpch.h"

#include "Bone.h"

namespace Pixel {
	Bone::Bone()
	{

	}

	Bone::~Bone()
	{

	}

	glm::mat4 Bone::GetBoneOffsetMatrix() const
	{
		return m_OffsetMatrix;
	}

	uint32_t Bone::GetChildrensNumber() const
	{
		return m_Childrens.size();
	}

	std::vector<Ref<Bone>>& Bone::GetChildrensArray()
	{
		return m_Childrens;
	}
}


