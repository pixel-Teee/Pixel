#include "pxpch.h"

#include "Animator.h"
#include "Animation.h"

namespace Pixel {

	Animator::Animator(Animation* animation)
	{
		m_CurrentTime = 0.0f;
		m_CurrentAnimation = animation;

		m_FinalBoneMatrices.reserve(100);

		for (int32_t i = 0; i < 100; ++i)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
	}

	void Animator::UpdateAnimation(float dt)
	{
		m_DeltaTime = dt;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			m_CurrentTime += fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());//loop the animation
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void Animator::PlayAnimation(Animation* pAnimation)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
	}

	void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;

		glm::mat4 nodeTransform = node->transformation;

		Bone* bone = m_CurrentAnimation->FindBone(nodeName);

		if (bone)
		{
			bone->Update(m_CurrentTime);
			nodeTransform = bone->GetLocalTransform();
		}

		glm::mat4 globalTransform = parentTransform * nodeTransform;

		//assign to m_FinalBoneMatrices
		auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int32_t index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			m_FinalBoneMatrices[index] = globalTransform * offset;
		}

		for (int32_t i = 0; i < node->childrenCount; ++i)
		{
			CalculateBoneTransform(&node->children[i], globalTransform);
		}
	}

}