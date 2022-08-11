#include "pxpch.h"

#include "SceneHierarchyPanel.h"

#include "Pixel/Scene/Components.h"
#include "Pixel/Core/Application.h"
#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"
#include "Pixel/Renderer/Descriptor/DescriptorAllocator.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorGpuHandle.h"
#include "Pixel/Renderer/Device/Device.h"
#include "Pixel/Renderer/Context/ContextManager.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "glm/gtc/type_ptr.hpp"

#include <filesystem>

namespace Pixel
{
	extern const std::filesystem::path g_AssetPath;

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	SceneHierarchyPanel::SceneHierarchyPanel()
	{
		//get the imgui srv heap
		m_MaterialComponentHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(5);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{
			m_Context->m_Registry.each([&](auto entityID)
				{
					Entity entity{ entityID, m_Context.get() };
					//DrawEntityNode(entity);
					if (entity.GetComponent<TransformComponent>().parentUUID == 0)
					{
						DrawEntityNode(entity);
					}
				}
			);

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			{
				m_SelectionContext = {};
			}

			//Right Click on Black Space
			if (ImGui::BeginPopupContextWindow(0, 1, false))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
				{
					Entity& newEntity = m_Context->CreateEntity("Empty Entity");
				}

				ImGui::EndPopup();
			}			
		}
		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}
		
		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::RecursiveDrawEntity(entt::entity entity, Ref<Scene> scene)
	{
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		auto& tag = scene->GetRegistry().get<TagComponent>(entity).Tag;

		const std::vector<UUID>& childrensUUID = scene->GetRegistry().get<TransformComponent>(entity).childrensUUID;

		//draw self
		if (ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str()))
		{
			//drag entity to parent entity
			if (ImGui::BeginDragDropSource())
			{
				uint32_t childrenEntityId = static_cast<uint32_t>(entity);
				ImGui::SetDragDropPayload("ChildrenEntity", &childrenEntityId, sizeof(uint32_t), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ChildrenEntity"))
				{
					uint32_t* childrenEntity = (uint32_t*)payload->Data;
					TransformComponent& transformComponent = scene->GetRegistry().get<TransformComponent>(static_cast<entt::entity>(*childrenEntity));//drag source

					if (transformComponent.parentUUID != 0)
					{
						//delete the link of the children entity and parent entity
						auto& UUIDs = scene->GetRegistry().view<IDComponent>();
						for (auto& UUIDOwner : UUIDs)
						{
							if (scene->GetRegistry().get<IDComponent>(UUIDOwner).ID == transformComponent.parentUUID)
							{
								//parent's children UUID
								auto& childrensUUID = scene->GetRegistry().get<TransformComponent>(UUIDOwner).childrensUUID;
								auto& iter = std::find(childrensUUID.begin(), childrensUUID.end(), scene->GetRegistry().get<IDComponent>(static_cast<entt::entity>(*childrenEntity)).ID);
								childrensUUID.erase(iter);//delete it
								break;
							}
						}					

						transformComponent.parentUUID = 0;
					}

					//create the new link of the children entity and current entity
					TransformComponent& currentTransformComponent = scene->GetRegistry().get<TransformComponent>(static_cast<entt::entity>(entity));
					UUID childrenUUID = scene->GetRegistry().get<IDComponent>(static_cast<entt::entity>(*childrenEntity)).ID;
					currentTransformComponent.childrensUUID.push_back(childrenUUID);
					transformComponent.parentUUID = scene->GetRegistry().get<IDComponent>(static_cast<entt::entity>(entity)).ID;
				}

				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Delete Entity"))
				{
					needDeltedEntity = Entity{ entity, scene.get() };
					m_entityDeleted = true;
				}

				ImGui::EndPopup();
			}

			if (ImGui::IsItemClicked())
			{
				m_SelectionContext = Entity{ entity, scene.get() };
			}

			//------from the all uuid to find the children's entity------
			auto& allUUIDs = scene->GetRegistry().view<IDComponent>();
			std::vector<entt::entity> neededToDrawEntity;
			for (auto& UUIDOwner : allUUIDs)
			{
				if (std::find(childrensUUID.begin(), childrensUUID.end(), scene->GetRegistry().get<IDComponent>(UUIDOwner).ID) != childrensUUID.end())
				{
					neededToDrawEntity.push_back(UUIDOwner);
				}
			}
			//------from the all uuid to find the children's entity------

			for (uint32_t i = 0; i < neededToDrawEntity.size(); ++i)
			{
				RecursiveDrawEntity(neededToDrawEntity[i], scene);
			}
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		m_entityDeleted = false;

		RecursiveDrawEntity(entity, m_Context);

		if (m_entityDeleted)
		{
			//find the link
			TransformComponent& currentTransformComponent = m_Context->GetRegistry().get<TransformComponent>(needDeltedEntity);
			
			//delete the link of the current entity and it's parent entity
			if (currentTransformComponent.parentUUID != 0)
			{
				auto& UUIDs = m_Context->GetRegistry().view<UUID>();
				for (auto& UUIDOwner : UUIDs)
				{
					UUID parentUUID = m_Context->GetRegistry().get<IDComponent>(UUIDOwner).ID;
					if (parentUUID == currentTransformComponent.parentUUID)
					{
						TransformComponent& parentTransformComponent = m_Context->GetRegistry().get<TransformComponent>(UUIDOwner);
						auto& iter = std::find(parentTransformComponent.childrensUUID.begin(), parentTransformComponent.childrensUUID.end(), m_Context->GetRegistry().get<IDComponent>(needDeltedEntity).ID);
						parentTransformComponent.childrensUUID.erase(iter);
						break;
					}
				}
				currentTransformComponent.parentUUID = 0;
			}		

			//delete the link of the current entity and it's children entity
			auto& UUIDs = m_Context->GetRegistry().view<IDComponent>();
			for (auto& UUIDOwner : UUIDs)
			{
				UUID childrenUUID = m_Context->GetRegistry().get<IDComponent>(UUIDOwner).ID;

				if (std::find(currentTransformComponent.childrensUUID.begin(), currentTransformComponent.childrensUUID.end(), childrenUUID) != currentTransformComponent.childrensUUID.end())
				{
					UUID& parentUUID = m_Context->GetRegistry().get<TransformComponent>(UUIDOwner).parentUUID;
					parentUUID = 0;
				}
			}

			m_Context->DestroyEntity(needDeltedEntity);

			if (m_SelectionContext == needDeltedEntity)
				m_SelectionContext = {};
		}
	}

	/*
		resetValue : when we press the label button, the value will be the resetValue
	*/
	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if(ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2(lineHeight, lineHeight)))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;
				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			static char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		//take the reset item width
		ImGui::PushItemWidth(-1);

		/*----------Add Component----------*/
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				m_SelectionContext.AddComponent<CameraComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Sprite Renderer"))
			{
				m_SelectionContext.AddComponent<SpriteRendererComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Rigidbody 2D"))
			{
				m_SelectionContext.AddComponent<Rigidbody2DComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("BoxCollider 2D"))
			{
				m_SelectionContext.AddComponent<BoxCollider2DComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Static Mesh Renderer"))
			{
				m_SelectionContext.AddComponent<StaticMeshComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Material"))
			{
				m_SelectionContext.AddComponent<MaterialComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Light"))
			{
				m_SelectionContext.AddComponent<LightComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Script"))
			{
				m_SelectionContext.AddComponent<NativeScriptComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Rigid3D"))
			{
				m_SelectionContext.AddComponent<RigidBody3DComponent>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		/*----------Add Component----------*/
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				DrawVec3Control("Scale", component.Scale);
			}
		);

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.camera;

				ImGui::Checkbox("Primary", &component.Primary);

				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; ++i)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float fov = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGui::DragFloat("fov", &fov))
					{
						camera.SetPerspectiveVerticalFOV(glm::radians(fov));
					}

					float nearClip = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &nearClip))
					{
						camera.SetPerspectiveNearClip(nearClip);
					}

					float farClip = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &farClip))
					{
						camera.SetPerspectiveFarClip(farClip);
					}
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
					{
						camera.SetOrthographicSize(orthoSize);
					}

					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
					{
						camera.SetOrthographicNearClip(orthoNear);
					}

					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
					{
						camera.SetOrthographicFarClip(orthoFar);
					}

					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
				}

				ImGui::Checkbox("DisplayFrustum", &component.DisplayFurstum);
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

				ImGui::Button("Texture", ImVec2(100.0f, 0.0f));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
						//component.Texture = Texture2D::Create(texturePath.string());
						
						//check the texture alread in asset manager?
						if (AssetManager::GetSingleton().IsInAssetRegistry(texturePath.string()))
						{
							component.Texture = AssetManager::GetSingleton().GetTexture(AssetManager::GetSingleton().GetAssetRegistryPath(texturePath.string()));
							//component.Path
						}
					}

					ImGui::EndDragDropTarget();
				}

				/*
				if (component.Texture)
				{
					ImGui::ImageButton((ImTextureID)component.Texture->GetRendererID(), ImVec2(16.0f, 16.0f));
				}
				*/

				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
			}
		);

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
			{
				const char* bodyTypeStrings[] = {"Static", "Dynamic", "Kinematic"};
				const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
				if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
				{
					for (int i = 0; i < 2; ++i)
					{
						bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
						if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
						{
							currentBodyTypeString = bodyTypeStrings[i];
							component.Type = (Rigidbody2DComponent::BodyType)i;
						}
						if(isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
			}
		);

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("RestitutionThreshold", &component.RestitutionThreshold, 0.01f, 0.0f, 1.0f);
			}
		);

		DrawComponent<StaticMeshComponent>("Static Mesh Renderer", entity, [](auto& component)
			{
				//ImGui::Combo("Mesh", &component.currentItem, component.path);
				
				ImGui::Text("Mesh:");
				ImGui::SameLine();
				ImGui::Button(component.path.c_str(), ImVec2(100.0f, 0.0f));
				//ImGui::Text
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						//const wchar_t* path = (const wchar_t*)payload->Data;
						//std::filesystem::path meshPath = std::filesystem::path(g_AssetPath) / path;
						//component.mesh = Model(meshPath.string());

						//component.mesh = AssetManager::GetSingleton().GetModel()

						/*
						std::string filepath = meshPath.string();
						auto lastSlash = filepath.find_last_of("/\\");
						lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
						auto lastDot = filepath.rfind('.');
						auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
						std::string Name = filepath.substr(lastSlash, count);
						*/
						//memcpy(component.path, meshPath.string().c_str(), meshPath.string().size());
						//component.path = meshPath.string();
						//memcpy(const_cast<char*>(component.path), const_cast<char*>(meshPath.string().c_str()), sizeof(char) * const_cast<char*>(meshPath.string().c_str()));
						const char* virtualPath = (const char*)payload->Data;

						component.m_Model = AssetManager::GetSingleton().GetModel(virtualPath);

						component.path = virtualPath;
					}

					ImGui::EndDragDropTarget();
				}
			}
		);
		
		DrawComponent<MaterialComponent>("Material Component", entity, [this](auto& component)
			{

				if(component.nextFrameNeedLoadTexture[0])
				{
					component.nextFrameNeedLoadTexture[0] = false;
					//component.Albedo = Texture2D::Create(component.albedoPath);
					component.Albedo = AssetManager::GetSingleton().GetTexture(component.albedoPath);
				}

				if (component.nextFrameNeedLoadTexture[1])
				{
					component.nextFrameNeedLoadTexture[1] = false;
					component.NormalMap = AssetManager::GetSingleton().GetTexture(component.normalMapPath);
				}

				if (component.nextFrameNeedLoadTexture[2])
				{
					component.nextFrameNeedLoadTexture[2] = false;
					component.Roughness = AssetManager::GetSingleton().GetTexture(component.roughnessPath);
				}

				if (component.nextFrameNeedLoadTexture[3])
				{
					component.nextFrameNeedLoadTexture[3] = false;
					component.Metallic = AssetManager::GetSingleton().GetTexture(component.metallicPath);
				}

				if (component.nextFrameNeedLoadTexture[4])
				{
					component.nextFrameNeedLoadTexture[4] = false;
					component.Emissive = AssetManager::GetSingleton().GetTexture(component.emissivePath);
				}

				//copy descriptor to imgui srv descriptor heap
				//Ref<DescriptorHeap> descriptorHeap = Application::Get().GetImGuiLayer()->GetSrvHeap();

				//get the texture descriptor handle, and copy to the srv heap
				std::vector<Ref<DescriptorCpuHandle>> pHandles(5);
				pHandles[0] = component.Albedo->GetHandle()->GetCpuHandle();
				pHandles[1] = component.NormalMap->GetHandle()->GetCpuHandle();
				pHandles[2] = component.Roughness->GetHandle()->GetCpuHandle();
				pHandles[3] = component.Metallic->GetHandle()->GetCpuHandle();
				pHandles[4] = component.Emissive->GetHandle()->GetCpuHandle();

				std::vector<DescriptorHandle> pDestHandles(5);
				uint32_t DescriptorSize = Device::Get()->GetDescriptorAllocator((uint32_t)DescriptorHeapType::CBV_UAV_SRV)->GetDescriptorSize();
				pDestHandles[0] = (*this->m_MaterialComponentHandle) ;
				pDestHandles[1] = (*this->m_MaterialComponentHandle) + DescriptorSize;
				pDestHandles[2] = (*this->m_MaterialComponentHandle) + 2 * DescriptorSize;
				pDestHandles[3] = (*this->m_MaterialComponentHandle) + 3 * DescriptorSize;
				pDestHandles[4] = (*this->m_MaterialComponentHandle) + 4 * DescriptorSize;

				for(uint32_t i = 0; i < 5; ++i)
					Device::Get()->CopyDescriptorsSimple(1, pDestHandles[i].GetCpuHandle(), pHandles[i], DescriptorHeapType::CBV_UAV_SRV);			

				ImGui::Text("Albedo");
				ImGui::Image((ImTextureID)(pDestHandles[0].GetGpuHandle()->GetGpuPtr()), ImVec2(128.0f, 128.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						//const wchar_t* path = (const wchar_t*)payload->Data;
						//std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
						////component.Albedo = Texture2D::Create(texturePath.string());
						//component.nextFrameNeedLoadTexture[0] = true;
						//component.albedoPath = texturePath.string();
						const char* virtualPath = (const char*)payload->Data;

						component.nextFrameNeedLoadTexture[0] = true;

						component.albedoPath = virtualPath;
					}

					ImGui::EndDragDropTarget();
				}
				ImGui::DragFloat3("Albedo Value:", glm::value_ptr(component.gAlbedo), 0.05f, 0.0f, 1.0f);
				//ImGui::InputFloat3("Albedo Value:", glm::value_ptr(component.gAlbedo));

				ImGui::Text("NormalMap");
				ImGui::Image((ImTextureID)pDestHandles[1].GetGpuHandle()->GetGpuPtr(), ImVec2(128.0f, 128.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const char* virtualPath = (const char*)payload->Data;

						component.nextFrameNeedLoadTexture[1] = true;

						component.normalMapPath = virtualPath;
					}

					ImGui::EndDragDropTarget();
				}
				ImGui::Checkbox("HaveNormal", &component.HaveNormal);
				//ImGui::DragFloat3("Normal Value:", glm::value_ptr(component.gNormal), 0.05f, 0.0f, 1.0f);
				//ImGui::InputFloat3("Normal Value:", glm::value_ptr(component.gNormal));

				ImGui::Text("Roughness");
				ImGui::Image((ImTextureID)pDestHandles[2].GetGpuHandle()->GetGpuPtr(), ImVec2(128.0f, 128.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const char* virtualPath = (const char*)payload->Data;

						component.nextFrameNeedLoadTexture[2] = true;

						component.roughnessPath = virtualPath;
					}

					ImGui::EndDragDropTarget();
				}
				ImGui::DragFloat("Roughness Value:", &component.gRoughness, 0.05f, 0.0f, 1.0f);
				//ImGui::InputFloat("Roughness Value:", &component.gRoughness);

				ImGui::Text("Metallic");
				ImGui::Image((ImTextureID)pDestHandles[3].GetGpuHandle()->GetGpuPtr(), ImVec2(128.0f, 128.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const char* virtualPath = (const char*)payload->Data;

						component.nextFrameNeedLoadTexture[3] = true;

						component.metallicPath = virtualPath;
					}

					ImGui::EndDragDropTarget();
				}
				ImGui::DragFloat("Metallic Value:", &component.gMetallic, 0.05f, 0.0f, 1.0f);
				//ImGui::InputFloat("Metallic Value:", &component.gMetallic);

				ImGui::Text("Emissive");
				ImGui::Image((ImTextureID)pDestHandles[4].GetGpuHandle()->GetGpuPtr(), ImVec2(128.0f, 128.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const char* virtualPath = (const char*)payload->Data;

						component.nextFrameNeedLoadTexture[4] = true;

						component.emissivePath = virtualPath;
					}

					ImGui::EndDragDropTarget();
				}
				ImGui::DragFloat("Emissive Value:", &component.gEmissive, 0.05f, 0.0f, 1.0f);
				//ImGui::InputFloat("Emissive Value:", &component.gEmissive);

				//ImGui::DragFloat("Shininess", &component.shininess, 2.0f, 2.0f, 64.0f);

				if (component.shadingModel == ShadingModel::SM_ClearCoat)
				{
					ImGui::DragFloat("ClearCoat", &component.ClearCoat, 0.05f, 0.0f, 1.0f);
					ImGui::DragFloat("ClearCoatRoughness", &component.ClearCoatRoughness, 0.05f, 0.0f, 1.0f);
				}

				//------Shading Model------
				const char* ShadingModelString[] = { "UnLit", "DefaultLit", "ClearCoat", "NRP"};
				const char* currentShadingModelString = ShadingModelString[(int)component.shadingModel];
				if (ImGui::BeginCombo("Shading Model", currentShadingModelString))
				{
					for (int i = 0; i < 4; ++i)
					{
						bool isSelected = currentShadingModelString == ShadingModelString[i];
						if (ImGui::Selectable(ShadingModelString[i], isSelected))
						{
							currentShadingModelString = ShadingModelString[i];
							component.shadingModel = (ShadingModel)i;
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				//------Shading Model------
			}
		);
		
		DrawComponent<LightComponent>("Light Component", entity, [](auto& component) 
			{
				const char* LightTypeString[] = { "PointLight", "DirectLight", "SpointLight" };
				const char* currentLightTypeString = LightTypeString[(int)component.lightType];
				if (ImGui::BeginCombo("Light Type", currentLightTypeString))
				{
					for (int i = 0; i < 3; ++i)
					{
						bool isSelected = currentLightTypeString == LightTypeString[i];
						if (ImGui::Selectable(LightTypeString[i], isSelected))
						{
							currentLightTypeString = LightTypeString[i];
							component.lightType = (LightType)i;
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				if (component.lightType == LightType::PointLight)
				{
					ImGui::ColorEdit3("Light color", glm::value_ptr(component.color));
					//ImGui::DragFloat("Light diffuse", &component.diffuse, 10.0f, 1.0f, 1000.0f);
					ImGui::DragFloat("Light constant", &component.constant, 0.02f, 0.02f, 1.0f, "%.2f");
					ImGui::DragFloat("Light linear", &component.linear, 0.02f, 0.02f, 1.0f, "%.2f");
					ImGui::DragFloat("Light quadratic", &component.quadratic, 0.0005f, 0.0070f, 2.0f, "%.4f");
					ImGui::Checkbox("DisplayPointLightVolume", &component.DisplayLightVolume);
				}
				else if (component.lightType == LightType::DirectLight)
				{
					ImGui::ColorEdit3("Light color", glm::value_ptr(component.color));
					ImGui::Checkbox("GenerateShadowMap", &component.GenerateShadowMap);
				}
				else if(component.lightType == LightType::SpotLight)
				{
					ImGui::ColorEdit3("Light color", glm::value_ptr(component.color));
					ImGui::DragFloat("Light CutOff", &component.CutOff, 1.0f, 1.0f, 90.0f, "%.2f");
				}
			}
		);

		DrawComponent<NativeScriptComponent>("Script Component", entity, [](auto& component)
			{
				char buf[256];
				memcpy(buf, component.m_path.c_str(), component.m_path.size());
				buf[component.m_path.size()] = '\0';
				if (ImGui::InputText("Script Path:", buf, 256))
				{
					component.m_path = buf;
				}
			}
		);

		DrawComponent<RigidBody3DComponent>("RigiBody3DComponent", entity, [](auto& component)
			{
				const char* ShapeTypeString[] = {"BoxShape", "SphereShape", "ConvexHull"};
				const char* currentShapeTypeString = ShapeTypeString[(int)component.m_shapeType];
				if (ImGui::BeginCombo("Shape Type", currentShapeTypeString))
				{
					for (int i = 0; i < 3; ++i)
					{
						bool isSelected = currentShapeTypeString == ShapeTypeString[i];
						if (ImGui::Selectable(ShapeTypeString[i], isSelected))
						{
							currentShapeTypeString = ShapeTypeString[i];
							component.m_shapeType = (RigidBody3DComponent::ShapeType)i;
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				ImGui::DragFloat("Mass", &component.m_bodyMass);
				ImGui::DragFloat3("BodyInertia", glm::value_ptr(component.m_bodyInertia));
				ImGui::DragFloat("Restitution", &component.m_restitution);
				ImGui::DragFloat("Friction", &component.m_friction);

				if (component.m_shapeType == RigidBody3DComponent::ShapeType::BoxShape)
				{
					ImGui::DragFloat("HalfLength", &component.m_HalfLength);
				}
				else if (component.m_shapeType == RigidBody3DComponent::ShapeType::SphereShape)
				{
					ImGui::DragFloat("Radius", &component.m_Radius);
				}
			}
		);
	}
}