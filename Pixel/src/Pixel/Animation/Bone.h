#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Pixel {
	struct Meta(Enable) KeyPosition
	{
		glm::vec3 Position;
		float TimeStep;
	};

	struct Meta(Enable) KeyRotation
	{
		glm::quat Orientation;
		float TimeStep;
	};

	struct Meta(Enable) KeyScale
	{
		glm::vec3 Scale;
		float TimeStep;
	};
	
	//bone, save the one animation's all key frame 
	class Bone {
	public:

	private:
		std::vector<KeyPosition> m_Positions;

		std::vector<KeyRotation> m_Rotations;

		std::vector<KeyScale> m_Scales;

		int32_t m_NumPositions;

		int32_t m_NumRotations;

		int32_t m_NumScalings;

		glm::mat4 m_LocalTransform;
		std::string m_Name;//bone name
		int32_t m_ID;//the transform matrices array's index
	};
}