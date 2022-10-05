#pragma once

namespace Pixel {
	class Bone;
	class Meta(Enable) Skeleton {
	public:
		Meta()
		Skeleton();

		~Skeleton();

		uint32_t GetBoneName() const;

		Ref<Bone> GetBone(const std::string& name) const;

		Ref<Bone> GetBone(uint32_t i) const;

		uint32_t GetBoneIndex(const std::string& name) const;
	private:
		//all bones
		std::vector<Ref<Bone>> m_BoneArray;

		friend class Model;
		friend class StaticMesh;
	};
}