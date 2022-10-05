#include "pxpch.h"

#include "Skeleton.h"

#include "Bone.h"

namespace Pixel {
	Skeleton::Skeleton()
	{

	}

	Skeleton::~Skeleton()
	{

	}

	uint32_t Skeleton::GetBoneName() const
	{
		return m_BoneArray.size();
	}

	Ref<Bone> Skeleton::GetBone(const std::string& name) const
	{
		for (size_t i = 0; i < m_BoneArray.size(); ++i)
		{
			if (m_BoneArray[i]->m_Name == name)
			{
				return m_BoneArray[i];
			}
		}

		PX_CORE_ASSERT(false, "couldn't find bone!");
		return nullptr;
	}

	Ref<Bone> Skeleton::GetBone(uint32_t i) const
	{
		PX_CORE_ASSERT(i < m_BoneArray.size(), "out of the range!");
		return m_BoneArray[i];
	}

	uint32_t Skeleton::GetBoneIndex(const std::string& name) const
	{
		for (size_t i = 0; i < m_BoneArray.size(); ++i)
		{
			if (m_BoneArray[i]->m_Name == name)
				return i;
		}
		PX_CORE_ASSERT(false, "couldn't find bone index!");
		return -1;
	}
}