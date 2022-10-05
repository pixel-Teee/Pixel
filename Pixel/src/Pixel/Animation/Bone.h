#pragma once

#include <glm/glm.hpp>

namespace Pixel {
	class Meta(Enable) Bone {
	public:
		Meta()
		Bone();

		~Bone();

		glm::mat4 GetBoneOffsetMatrix() const;

		uint32_t GetChildrensNumber() const;

		std::vector<Ref<Bone>>& GetChildrensArray();
	private:
		std::string m_Name;//bone name

		glm::mat4 m_OffsetMatrix;//offset matrix

		//vector is dynamic allocator, bone's children bone
		std::vector<Ref<Bone>> m_Childrens;

		friend class Skeleton;
		friend class Model;
		friend class StaticMesh;
	};
}