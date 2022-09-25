#pragma once

#include <glm/glm.hpp>

namespace Pixel {
	class Animation;
	struct AssimpNodeData;
	class Animator {
	public:
		Animator(Animation* animation);

		void UpdateAnimation(float dt);

		void PlayAnimation(Animation* pAnimation);

		//drive bone to update, AssimpNodeData is hierarchy data
		void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

		std::vector<glm::mat4> GetFinalBoneMatrices() { return m_FinalBoneMatrices; }
	private:
		std::vector<glm::mat4> m_FinalBoneMatrices;

		Animation* m_CurrentAnimation;//animator reference the animation

		float m_CurrentTime;

		float m_DeltaTime;
	};
}