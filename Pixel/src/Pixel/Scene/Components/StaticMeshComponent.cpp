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
}