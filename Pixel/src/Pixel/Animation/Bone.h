#pragma once

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

struct aiNodeAnim;
namespace Pixel {
	struct Meta(Enable) KeyPosition
	{
		Meta()
		glm::vec3 Position;
		Meta()
		float TimeStep;
	};

	struct Meta(Enable) KeyRotation
	{
		Meta()
		glm::quat Orientation;
		Meta()
		float TimeStep;
	};

	struct Meta(Enable) KeyScale
	{
		Meta()
		glm::vec3 Scale;
		Meta()
		float TimeStep;
	};
	
	//bone, save the one animation's all key frame 
	class Meta(Enable) Bone {
	public:
		Meta()
		Bone() = default;
		//aiNodeAnim represents a Bone
		//read keyframes from aiNodeAnim*
		Bone(const std::string & name, int32_t ID, const aiNodeAnim * channel);

		//interpolates b/w positions & scaling keys based on the current time of the animation
		//and prepares the local transformation matrix by combining all keys transformations
		void Update(float animtionTime);

		glm::mat4 GetLocalTransform();

		std::string GetBoneName() const;

		int32_t GetBoneID();

		int32_t GetPositionIndex(float animationTime);

		int32_t GetRotationIndex(float animationTime);

		int32_t GetScaleIndex(float animationTime);

		//gets normalized value for lerp & slerp
		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

		glm::mat4 InterpolatePosition(float animationTime);

		glm::mat4 InterpolateRotation(float animationTime);

		glm::mat4 InterpolateScaling(float animationTime);

	private:
		Meta()
		std::vector<KeyPosition> m_Positions;
		Meta()
		std::vector<KeyRotation> m_Rotations;
		Meta()
		std::vector<KeyScale> m_Scales;
		Meta()
		int32_t m_NumPositions;
		Meta()
		int32_t m_NumRotations;
		Meta()
		int32_t m_NumScalings;

		glm::mat4 m_LocalTransform;
		Meta()
		std::string m_Name;//bone name
		Meta()
		int32_t m_ID;//the transform matrices array's index

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}