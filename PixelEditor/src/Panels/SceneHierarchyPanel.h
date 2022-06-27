#pragma once

#include "Pixel/Core/Core.h"
#include "Pixel/Scene/Scene.h"
#include "Pixel/Scene/Entity.h"

namespace Pixel
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel();
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const {return m_SelectionContext;}
		void SetSelectedEntity(Entity entity);
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;

		//aim:for material texture component handle
		Ref<DescriptorHandle> m_MaterialComponentHandle;
	};
}