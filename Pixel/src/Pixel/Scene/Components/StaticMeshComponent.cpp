#include "pxpch.h"

//------my library------
#include "StaticMeshComponent.h"
#include "Pixel/Asset/AssetManager.h"
#include "Pixel/Renderer/3D/Model.h"
//------my library------

namespace Pixel
{
	REFLECT_STRUCT_BEGIN(StaticMeshComponent)
	REFLECT_STRUCT_MEMBER(path)
	REFLECT_STRUCT_END()

	void StaticMeshComponent::PostLoad()
	{
		m_Model = AssetManager::GetSingleton().GetModel(path);
	}

}