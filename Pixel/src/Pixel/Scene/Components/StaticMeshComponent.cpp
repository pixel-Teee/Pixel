#include "pxpch.h"

//------my library------
#include "StaticMeshComponent.h"
#include "Pixel/Asset/AssetManager.h"
#include "Pixel/Renderer/3D/Model.h"
//------my library------

namespace Pixel
{

	void StaticMeshComponent::PostLoad()
	{
		m_Model = AssetManager::GetSingleton().GetModel(path);
	}

	RTTR_REGISTRATION
	{
		using namespace rttr;
		registration::class_<StaticMeshComponent>("StaticMeshComponent")
			.constructor<>()
			.property("path", &StaticMeshComponent::path);
	}
}