#pragma once

#include "Bone.h"
#include "BoneInfo.h"

struct aiAnimation;
struct aiNode;
namespace Pixel {
	//come from the assimp animation data
	struct AssimpNodeData
	{
		glm::mat4 transformation;
		std::string name;
		int32_t childrenCount;
		//vector is dynamic allocator
		std::vector<AssimpNodeData> children;
	};

	class Model;
	class Meta(Enable) Animation {
	public:
		Meta()
		Animation() = default;

		//actual animation physical path
		Animation(const std::string & animationPath, Model * model);

		~Animation();

		//in terms of the name to find bone
		Bone* FindBone(const std::string & name);

		inline float GetTicksPerSecond() { return m_TicksPerSecond; }

		inline float GetDuration() { return m_Duration; }

		inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

		inline const std::map<std::string, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

	private:
		void ReadHeirarchyData(AssimpNodeData & dest, const aiNode * src);

		void ReadMissingBones(const aiAnimation* animation, Model& model);

		Meta()
		float m_Duration;
		
		Meta()
		int32_t m_TicksPerSecond;
		Meta()
		//save the every bones, every bone save the every key frame data
		std::vector<Bone> m_Bones;
		
		//bone info is id and offset matrix
		std::map<std::string, BoneInfo> m_BoneInfoMap;

		AssimpNodeData m_RootNode;
	};
}