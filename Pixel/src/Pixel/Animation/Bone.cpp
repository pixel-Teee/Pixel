#include "pxpch.h"

#include "Bone.h"

#include <assimp/anim.h>

namespace Pixel {
	//aiNodeAnim contains one bone
	Bone::Bone(const std::string& name, int32_t ID, const aiNodeAnim* channel)
		:m_Name(name), m_ID(ID), m_LocalTransform(1.0f)
	{
		//id is the final transform matrices index
		m_NumPositions = channel->mNumPositionKeys;

		for (int32_t positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
		{
			aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
			float timeStamp = channel->mPositionKeys[positionIndex].mTime;
			KeyPosition data;
			//data.Position = 
			data.TimeStep = timeStamp;
			m_Positions.push_back(data);
		}

		m_NumRotations = channel->mNumRotationKeys;
		for (int32_t rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
		{
			aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
			float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
			KeyRotation data;
			//data.orientation
			data.TimeStep = timeStamp;
			m_Rotations.push_back(data);
		}

		m_NumScalings = channel->mNumScalingKeys;
		for (int32_t keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
		{
			aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
			float timeStamp = channel->mScalingKeys[keyIndex].mTime;
			KeyScale data;
			//data.scale = 
			data.TimeStep = timeStamp;
			m_Scales.push_back(data);
		}
	}

	void Bone::Update(float animtionTime)
	{

		//translation rotation scale
		
	}

	glm::mat4 Bone::GetLocalTransform()
	{
		return m_LocalTransform;
	}

	std::string Bone::GetBoneName() const
	{
		return m_Name;
	}

	int32_t Bone::GetBoneID()
	{
		return m_ID;
	}

	int32_t Bone::GetPositionIndex(float animationTime)
	{
		for (int32_t index = 0; index < m_NumPositions - 1; ++index)
		{
			if (animationTime < m_Positions[index + 1].TimeStep)
				return index;
		}
		PX_CORE_ASSERT(false, "could not find the position index!");
	}

	int32_t Bone::GetRotationIndex(float animationTime)
	{
		for (int32_t index = 0; index < m_NumRotations - 1; ++index)
		{
			if (animationTime < m_Rotations[index + 1].TimeStep)
				return index;
		}
		PX_CORE_ASSERT(false, "could not find the rotation index!");
	}

	int32_t Bone::GetScaleIndex(float animationTime)
	{
		for (int32_t index = 0; index < m_NumScalings - 1; ++index)
		{
			if (animationTime < m_Scales[index + 1].TimeStep)
				return index;
		}
		PX_CORE_ASSERT(false, "could not find the scale index!");
	}

	float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		float scaleFactor = 0.0f;
		float midWayLength = animationTime - lastTimeStamp;
		float framsDiff = nextTimeStamp - lastTimeStamp;
		scaleFactor = midWayLength / framsDiff;
		return scaleFactor;
	}

	glm::mat4 Bone::InterpolatePosition(float animationTime)
	{
		if (m_NumPositions == 1)
			return glm::translate(glm::mat4(1.0f), m_Positions[0].Position);

		int32_t p0Index = GetPositionIndex(animationTime);
		int32_t p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Positions[p0Index].TimeStep, m_Positions[p1Index].TimeStep, animationTime);
		
		//interpolate
		glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].Position, m_Positions[p1Index].Position, scaleFactor);

		return glm::translate(glm::mat4(1.0f), finalPosition);
	}

	glm::mat4 Bone::InterpolateRotation(float animationTime)
	{
		if (m_NumRotations == 1)
		{
			auto rotation = glm::normalize(m_Rotations[0].Orientation);
			return glm::toMat4(rotation);
		}

		int32_t p0Index = GetRotationIndex(animationTime);
		int32_t p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Rotations[p0Index].TimeStep, m_Rotations[p1Index].TimeStep, animationTime);

		glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].Orientation, m_Rotations[p1Index].Orientation, scaleFactor);
	}

	glm::mat4 Bone::InterpolateScaling(float animationTime)
	{
		if (m_NumScalings == 1)
			return glm::scale(glm::mat4(1.0f), m_Scales[0].Scale);

		int32_t p0Index = GetScaleIndex(animationTime);
		int32_t p1Index = p0Index + 1;
		float scaleFactor = GetScaleFactor(m_Scales[p0Index].TimeStep, m_Scales[p1Index].TimeStep, animationTime);

		glm::vec3 finalScale = glm::mix(m_Scales[p0Index].Scale, m_Scales[p1Index].Scale, scaleFactor);
		return glm::scale(glm::mat4(1.0f), finalScale);
	}
}


