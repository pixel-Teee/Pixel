#include "pxpch.h"

#include "Animation.h"

#include "Pixel/Renderer/3D/Model.h"

namespace Pixel {
	Animation::Animation(const std::string& animationPath, Model* model)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
		PX_CORE_ASSERT(scene && scene->mRootNode, "assimp scene is null!");
		auto animation = scene->mAnimations[0];//get the first animation
		m_Duration = animation->mDuration;
		m_TicksPerSecond = animation->mTicksPerSecond;
	}

	Animation::~Animation()
	{

	}

	void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
	{
		PX_CORE_ASSERT(src, "aiNode is nullptr!");

		dest.name = src->mName.data;
		//dest.transformation =
		dest.childrenCount = src->mNumChildren;

		for (int32_t i = 0; i < src->mNumChildren; ++i)
		{
			AssimpNodeData newData;
			ReadHeirarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}

	Bone* Animation::FindBone(const std::string& name)
	{
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(), [&](const Bone& bone) { return bone.GetBoneName() == name; });

		if (iter == m_Bones.end()) return nullptr;
		else return &(*iter);
	}

	void Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
	{
		//get bones number
		int32_t size = animation->mNumChannels;

		auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
		int32_t boneCount = model.GetBoneCount();//getting m_BoneCounter from Model class

		//reading channels(bones engaged in an animation and their keyframes)
		for (int32_t i = 0; i < size; ++i)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;//get the bone name

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				++boneCount;
			}
			//construct bone
			m_Bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
		}

		m_BoneInfoMap = boneInfoMap;
	}
}