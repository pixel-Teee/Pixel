#include "pxpch.h"

#include "NativeScriptComponent.h"
#include "Pixel/Scene/ScriptableEntity.h"

namespace Pixel {

	void NativeScriptComponent::Instantiate(std::string path)
	{
		Instance = new ScriptableEntity();
		Instance->m_path = path;
	}

	void NativeScriptComponent::Destroy()
	{
		delete Instance;
	}

}