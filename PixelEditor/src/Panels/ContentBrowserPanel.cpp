#include "pxpch.h"
#include "ContentBrowserPanel.h"

#include "Pixel/Core/Application.h"
#include "Pixel/Renderer/Descriptor/DescriptorHeap.h"
#include "Pixel/Renderer/Device/Device.h"
#include "Pixel/Renderer/DescriptorHandle/DescriptorGpuHandle.h"

//------my library------
#include "Pixel/Asset/AssetManager.h"
#include "Pixel/Utils/PlatformUtils.h"
#include "Pixel/Scene/Components/MaterialComponent.h"
#include "Pixel/Renderer/Device/Device.h"
#include "Pixel/Renderer/Descriptor/DescriptorAllocator.h"
#include "Pixel/Renderer/3D/Material/Material.h"
#include "Pixel/Renderer/3D/Material/MaterialInstance.h"
#include "Pixel/Renderer/3D/Material/ShaderMainFunction.h"
#include "Pixel/Renderer/3D/Material/InputNode.h"
#include "Pixel/Renderer/3D/Material/ConstFloatValue.h"
#include "Pixel/Renderer/3D/Material/Mul.h"
#include "Pixel/Scene/SceneSerializer.h"
//------my library------

//------other library------
#include <imgui/imgui.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include "glm/gtc/type_ptr.hpp"
//------other library------

namespace Pixel {

	//once we have projects hub, change this
	extern const std::filesystem::path g_AssetPath = "assets";

	static std::string extractFileName(const std::string& physicalPath)
	{
		//std::string result;

		std::string fileName;

		size_t pos = physicalPath.find_last_of("/\\");
		if (pos == std::string::npos)
		{
			fileName = physicalPath;
		}
		else
		{
			fileName = physicalPath.substr(pos + 1);
		}

		size_t dotPos = fileName.find_last_of('.');

		if (dotPos == std::string::npos)
			return fileName;
		else
		{
			fileName = fileName.substr(0, dotPos);
			return fileName;
		}
	}

	ContentBrowserPanel::ContentBrowserPanel()
	:m_CurrentDirectory(g_AssetPath)
	{
		m_Directory = Texture2D::Create(g_AssetPath.string() + "/icons/directory.png");
		m_File = Texture2D::Create(g_AssetPath.string() + "/icons/file.png");

		//copy the these thexture's descriptor to descriptor heap
		m_DirectoryHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(1);
		m_FileHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(1);
		//TODO:need to remove this in the future
		m_MaterialAssetTextureHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(5);

		//temporarily is 256
		m_MaterialAssertTextureFirstHandle = Application::Get().GetImGuiLayer()->GetSrvHeap()->Alloc(256);
		m_CurrentMaterialAssertTextureHandleOffset = 0;
		
		Device::Get()->CopyDescriptorsSimple(1, m_DirectoryHandle->GetCpuHandle(), m_Directory->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, m_FileHandle->GetCpuHandle(), m_File->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);

		m_IsOpen = false;
		m_DelayToLoadImageNextFrame = false;
		//m_IsOpenTestMaterialEditor = false;
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		//list all the files in the assets directory
		ImGui::Begin("Content Browser");
		
		if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		if (ImGui::IsMouseClicked(1) && ImGui::IsWindowFocused())
			ImGui::OpenPopup("Content Browser");

		if (ImGui::BeginPopup("Content Browser"))
		{
			/*if (ImGui::MenuItem("Create Material Asset"))
			{
				CreateMaterialAsset(m_CurrentDirectory.string());
			}*/

			if (ImGui::MenuItem("Import Model"))
			{
				//create model
				std::wstring physicalFilePath = FileDialogs::OpenFile(L"model(*.fbx)\0*.fbx\0model(*.obj)\0*.obj\0model(*.gltf)\0*.gltf\0model(*.glb)\0*.glb\0", std::filesystem::absolute(m_CurrentDirectory).wstring().c_str());
				AssetManager::GetSingleton().AddModelToAssetRegistry(AssetManager::GetSingleton().to_string(physicalFilePath));
			}

			if (ImGui::MenuItem("Import Texture"))
			{
				std::wstring physicalFilePath = FileDialogs::OpenFile(L"texture(*.jpg)\0*.jpg\0texture(*.png)\0*.png\0texture(*.hdr)\0*.hdr\0", std::filesystem::absolute(m_CurrentDirectory).wstring().c_str());
				AssetManager::GetSingleton().AddTextureToAssetRegistry(physicalFilePath);
			}

			if (ImGui::MenuItem("Create Material"))
			{
				std::wstring physicalFilePath = FileDialogs::SaveFile(L"material(*.mat)\0*.mat\0", std::filesystem::absolute(m_CurrentDirectory).wstring().c_str());

				Ref<SubMaterial> pSubMaterial = CreateRef<SubMaterial>();

				//write a default material and add to asset registry
				AssetManager::GetSingleton().CreateSubMaterial(AssetManager::GetSingleton().to_string(physicalFilePath), pSubMaterial);

				AssetManager::GetSingleton().AddMaterialToAssetRegistry(physicalFilePath);
			}

			if (ImGui::MenuItem("Create Test Material"))
			{
				std::wstring physicalFilePath = FileDialogs::SaveFile(L"test material(*.tmat)\0*.tmat\0", std::filesystem::absolute(m_CurrentDirectory).wstring().c_str());

				Ref<Material> pMaterial = CreateRef<Material>();
				//pMaterial->GetMainFunction()->ConstructPutNodeAndSetPutNodeOwner();
				pMaterial->m_pShaderMainFunction = CreateRef<ShaderMainFunction>();
				pMaterial->GetMainFunction()->ConstructPutNodeAndSetPutNodeOwner();
				pMaterial->AddShaderFunction(pMaterial->m_pShaderMainFunction);
				pMaterial->GetMainFunction()->SetFunctionNodeId(1);
				pMaterial->GetMainFunction()->GetInputNode(0)->SetPutNodeId(2);
				pMaterial->GetMainFunction()->GetInputNode(1)->SetPutNodeId(3);
				pMaterial->GetMainFunction()->GetInputNode(2)->SetPutNodeId(4);
				pMaterial->GetMainFunction()->GetInputNode(3)->SetPutNodeId(5);
				pMaterial->GetMainFunction()->GetInputNode(4)->SetPutNodeId(6);
				//pMaterial->SetMaterialName(AssetManager::GetSingleton().GetVirtualPath(AssetManager::GetSingleton().to_string(physicalFilePath)));

				//extract the material name
				std::string materialName = extractFileName(AssetManager::GetSingleton().to_string(physicalFilePath));

				pMaterial->SetMaterialName(materialName);

				AssetManager::GetSingleton().CreateTestMaterial(AssetManager::GetSingleton().to_string(physicalFilePath), pMaterial);

				AssetManager::GetSingleton().AddTestMaterialToAssetRegistry(physicalFilePath);	

				//may be covering previous already loaded material
				std::map<std::string, Ref<Material>>& materials = AssetManager::GetSingleton().GetMaterials();

				//------get the relative physical path------
				std::filesystem::path PhysicalPath(AssetManager::GetSingleton().to_string(physicalFilePath));

				auto relativePath = std::filesystem::relative(PhysicalPath, g_AssetPath);

				std::string relativePathString = relativePath.string();
				//------get the relative physical path------
				
				std::string virtualPath = AssetManager::GetSingleton().GetVirtualPath(relativePathString);

				for (auto& item : materials)
				{
					if (item.first == virtualPath)
					{
						item.second = pMaterial;
						//need to notify material editor?
						break;
					}
				}
			}
			ImGui::EndPopup();
		}

		float ContentWidth = ImGui::GetContentRegionAvailWidth();

		static float Padding = 16.0f;
		static float ThumbnailSize = 128.0f;

		int CellNumber = ContentWidth / (Padding + ThumbnailSize);

		if(CellNumber < 1)
			CellNumber = 1;

		ImGui::Columns(CellNumber, 0, false);

		m_CurrentMaterialAssertTextureHandleOffset = 0;//reset this offset

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{		
			const auto& path = directoryEntry.path();
			//ImGui::Text("%s", path.string().c_str());
			auto relativePath = std::filesystem::relative(path, g_AssetPath);
			//ImGui::Text("%s", relativePath.string().c_str());
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());

			bool isDirectory = directoryEntry.is_directory();
			bool isInAssetRegistry = AssetManager::GetSingleton().IsInAssetRegistry(relativePath.string());

			if (isDirectory)
			{
				//check this whether is directory or file?
				Ref<Texture2D> icon = directoryEntry.is_directory() ? m_Directory : m_File;

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				ImGui::ImageButton((ImTextureID)(m_DirectoryHandle->GetGpuHandle()->GetGpuPtr()), { ThumbnailSize, ThumbnailSize }, { 0, 1 }, { 1, 0 });
				ImGui::PopStyleColor();

				if (ImGui::BeginDragDropSource())
				{
					//const wchar_t* itemPath = relativePath.c_str();
					const std::string& itemPath = relativePath.string();

					//query the asset virtual path
					const std::string& virtualPath = AssetManager::GetSingleton().GetVirtualPath(itemPath);

					//pass the asset virtual path
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", virtualPath.c_str(), strlen(virtualPath.c_str()) * sizeof(char), ImGuiCond_Once);

					ImGui::EndDragDropSource();
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (directoryEntry.is_directory())
					{
						m_CurrentDirectory /= path.filename();
					}
				}

				ImGui::TextWrapped("%s", filenameString.c_str());
				ImGui::NextColumn();
			}
			else if (!isDirectory && isInAssetRegistry)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

				const std::string& itemPath = relativePath.string();

				if (AssetManager::GetSingleton().IsInTestMaterialAssetRegistry(AssetManager::GetSingleton().GetVirtualPath(itemPath)))
				{
					std::string& virtualPath = AssetManager::GetSingleton().GetVirtualPath(itemPath);

					Ref<Material> pMaterial = AssetManager::GetSingleton().GetTestMaterial(virtualPath);

					std::string previewImagePhysicalPath = "assets/materials/preview/" + pMaterial->m_MaterialName + ".jpg";
					//get material name
					//m_GenerateThumbNail(virtualPath, pMaterial);
					if (!std::filesystem::exists(previewImagePhysicalPath))
					{
						m_GenerateThumbNail(pMaterial);
						m_DelayToLoadImageNextFrame = true;//when complete generate a nhumbnail, to load a image
					}
					else
					{
						//to load image
						std::map<std::string, Ref<Texture2D>>& previewImages = AssetManager::GetSingleton().GetMaterialPreviewImages();
						//check asset manager already load the preview image?
						if (previewImages.find(previewImagePhysicalPath) == previewImages.end())
						{
							previewImages[previewImagePhysicalPath] = Texture2D::Create(previewImagePhysicalPath);
						}
					}
					if (!m_DelayToLoadImageNextFrame)
					{
						//------delay to next frame to load image------
						//paste thumbnail image
						std::map<std::string, Ref<Texture2D>>& previewImages = AssetManager::GetSingleton().GetMaterialPreviewImages();
						Ref<Texture2D> previewImage = previewImages[previewImagePhysicalPath];

						++m_CurrentMaterialAssertTextureHandleOffset;
						uint32_t DescriptorSize = Device::Get()->GetDescriptorAllocator((uint32_t)DescriptorHeapType::CBV_UAV_SRV)->GetDescriptorSize();
						/*
						std::vector<DescriptorHandle> handles;
						for (uint32_t i = 0; i < 5; ++i)
						{
							DescriptorHandle handle = (*m_MaterialAssertTextureFirstHandle) + DescriptorSize;

							handles.push_back(handle);
						}
						*/

						DescriptorHandle handle = (*m_MaterialAssertTextureFirstHandle) + DescriptorSize * m_CurrentMaterialAssertTextureHandleOffset;
						//copy descriptor to descriptor heap
						Device::Get()->CopyDescriptorsSimple(1, handle.GetCpuHandle(), previewImage->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);

						ImGui::ImageButton((ImTextureID)(handle.GetGpuHandle()->GetGpuPtr()), { ThumbnailSize, ThumbnailSize }, { 0, 1 }, { 1, 0 });
						//------delay to next frame to load image------
					}
				}
				else
					ImGui::ImageButton((ImTextureID)(m_FileHandle->GetGpuHandle()->GetGpuPtr()), { ThumbnailSize, ThumbnailSize }, { 0, 1 }, { 1, 0 });
			
				if (ImGui::IsItemClicked(1))
				{
					const std::string& itemPath = relativePath.string();
					//if (AssetManager::GetSingleton().IsInTestMaterialAssetRegistry(AssetManager::GetSingleton().GetVirtualPath(itemPath)))
					//{
					//	//create material instance
					//	ImGui::OpenPopup("##CreateMaterialInstance");
					//}

					if (AssetManager::GetSingleton().IsInAssetRegistry(itemPath))
					{
						//right click
						ImGui::OpenPopup("##RightClickAsset");
					}
				}
				if (ImGui::IsItemClicked(0))//open material instance
				{
					//TODO:need to fix encapsulate a function
					const std::string& itemPath = relativePath.string();
					if (AssetManager::GetSingleton().IsInMaterialInstanceAssetRegistry(AssetManager::GetSingleton().GetVirtualPath(itemPath)))
					{
						//open material instance editor panel

					}
				}
				ImGui::PopStyleColor();


				if (ImGui::BeginPopup("##RightClickAsset"))
				{
					const std::string& itemPath = relativePath.string();
					if (AssetManager::GetSingleton().IsInTestMaterialAssetRegistry(AssetManager::GetSingleton().GetVirtualPath(itemPath)))
					{
						if (ImGui::MenuItem("CreateMaterialInstance"))
						{
							const std::string& itemPath = relativePath.string();
							//create material instance
							CreateMaterialInstance(AssetManager::GetSingleton().GetVirtualPath(itemPath));
						}
					}
					
					//from asset registry to delete it
					//TODO:find all reference it's assets, to notify them
					if (AssetManager::GetSingleton().IsInAssetRegistry(itemPath))
					{
						//if delete material, then will be delete material instance
						if (ImGui::MenuItem("DeleteAsset"))
						{
							AssetManager::GetSingleton().DeleteRegistry(AssetManager::GetSingleton().GetVirtualPath(itemPath));
						}				
					}

					ImGui::EndPopup();
				}

				if (ImGui::BeginDragDropSource())
				{
					//const wchar_t* itemPath = relativePath.c_str();
					const std::string& itemPath = relativePath.string();

					//query the asset virtual path
					const std::string& virtualPath = AssetManager::GetSingleton().GetVirtualPath(itemPath);

					//PIXEL_CORE_INFO("{0}", strlen(virtualPath.c_str()) * sizeof(char));

					//pass the asset virtual path
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", virtualPath.c_str(), (strlen(virtualPath.c_str()) + 1) * sizeof(char), ImGuiCond_Once);

					ImGui::EndDragDropSource();
				}

				if(ImGui::IsItemClicked(0))
				{
					const std::string& itemPath = relativePath.string();
					//check the file type, and open different editor
					if (AssetManager::GetSingleton().IsInMaterialAssetRegistry(AssetManager::GetSingleton().GetVirtualPath(itemPath)))
					{
						const std::string& materialPhysicalPath = g_AssetPath.string() + "\\" + relativePath.string();

						if (m_CurrentSubMaterialPath != materialPhysicalPath)
						{
							m_CurrentSubMaterialPath = materialPhysicalPath;

							//read the sub material from the material path
							m_pSubMaterial = CreateRef<SubMaterial>();
							//Reflect::TypeDescriptor* typeDesc = Reflect::TypeResolver<SubMaterial>::get();

							rttr::type subMaterialType = rttr::type::get<SubMaterial>();

							rapidjson::Document doc;

							std::ifstream stream(materialPhysicalPath);
							std::stringstream strStream;
							strStream << stream.rdbuf();
							if (!doc.Parse(strStream.str().data()).HasParseError())
							{
								//read the sub material
								if (doc.HasMember(subMaterialType.get_name().to_string().c_str()) && doc[subMaterialType.get_name().to_string().c_str()].IsObject())
								{
									SceneSerializer::FromJsonRecursive(*m_pSubMaterial, doc[subMaterialType.get_name().to_string().c_str()], false);
								}
							}
							stream.close();

							//post load the sub material
							m_pSubMaterial->PostLoad();

							//from the m_pSubMaterial texture handle, copy to imgui layer's srv heap
							uint32_t DescriptorSize = Device::Get()->GetDescriptorAllocator((uint32_t)DescriptorHeapType::CBV_UAV_SRV)->GetDescriptorSize();

							std::vector<DescriptorHandle> handles;
							for (uint32_t i = 0; i < 5; ++i)
							{
								DescriptorHandle handle = (*m_MaterialAssetTextureHandle) + i * DescriptorSize;

								handles.push_back(handle);
							}

							Device::Get()->CopyDescriptorsSimple(1, handles[0].GetCpuHandle(), m_pSubMaterial->albedoMap->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
							Device::Get()->CopyDescriptorsSimple(1, handles[1].GetCpuHandle(), m_pSubMaterial->normalMap->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
							Device::Get()->CopyDescriptorsSimple(1, handles[2].GetCpuHandle(), m_pSubMaterial->metallicMap->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
							Device::Get()->CopyDescriptorsSimple(1, handles[3].GetCpuHandle(), m_pSubMaterial->roughnessMap->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
							Device::Get()->CopyDescriptorsSimple(1, handles[4].GetCpuHandle(), m_pSubMaterial->aoMap->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
						}

						m_IsOpen = true;
					}

					if (AssetManager::GetSingleton().IsInTestMaterialAssetRegistry(AssetManager::GetSingleton().GetVirtualPath(itemPath)))
					{
						const std::string& materialPhysicalPath = g_AssetPath.string() + "\\" + relativePath.string();

						m_CurrentTestMaterialPath = materialPhysicalPath;

						//read the test material from the material path
						m_pMaterial = AssetManager::GetSingleton().GetTestMaterial(AssetManager::GetSingleton().GetVirtualPath(itemPath));

						int32_t slashPos = itemPath.find_last_of('\\');
						std::string editorPathFileName = itemPath.substr(std::min((unsigned long long)(slashPos + 1), itemPath.size() - 1));

						int32_t dotPos = editorPathFileName.find_last_of(".");
						std::string graphNodeEditorPath;
						if (dotPos != std::string::npos)
						{
							graphNodeEditorPath = editorPathFileName.substr(0, dotPos);
						}
						else
						{
							graphNodeEditorPath = editorPathFileName;
						}

						//notify editor layer to open graph editor
						m_OpenGraphEditor(graphNodeEditorPath, materialPhysicalPath, m_pMaterial);
						m_IsGraphEditorAlive(true);
						//m_GraphNodeEditor = CreateRef<GraphNodeEditor>(graphNodeEditorPath, materialPhysicalPath, m_pMaterial, m_pFramebuffer);
					}

					if (AssetManager::GetSingleton().IsInMaterialInstanceAssetRegistry(AssetManager::GetSingleton().GetVirtualPath(itemPath)))
					{
						//open material instance editor
						m_OpenMaterialInstanceEditor(AssetManager::GetSingleton().GetVirtualPath(itemPath));
					}
				}

				ImGui::TextWrapped("%s", filenameString.c_str());
				ImGui::NextColumn();
			}

			ImGui::PopID();
		}
		ImGui::Columns(1);
		ImGui::SliderFloat("ThumbnailSize", &ThumbnailSize, 16.0f, 512.0f);
		ImGui::SliderFloat("Padding", &Padding, 2.0f, 16.0f);

		ImGui::End();

		/*
		if(m_IsOpen)
		{
			RenderMaterialAssetPanel();
		}
		*/

		//if (m_IsOpenTestMaterialEditor)
		//{
		//	m_GraphNodeEditor->OnImGuiRender(m_IsOpenTestMaterialEditor);
		//}

		m_DelayToLoadImageNextFrame = false;
	}

	void ContentBrowserPanel::RegisterOpenGraphEditorCallBack(std::function<void(const std::string& virtualPath, const std::string& physicalPath, Ref<Material> pMaterial)> func)
	{
		//register, TODO:need to remove register on destory?
		m_OpenGraphEditor = func;
	}

	void ContentBrowserPanel::RegisterIsGraphEditorAliveCallBack(std::function<void(bool)> func)
	{
		m_IsGraphEditorAlive = func;
	}

	void ContentBrowserPanel::RegisterGenerateThumbNail(std::function<void(Ref<Material>pMaterial)> func)
	{
		m_GenerateThumbNail = func;
	}

	void ContentBrowserPanel::RegisterOpenMaterialInstanceEditor(std::function<void(const std::string&)> func)
	{
		m_OpenMaterialInstanceEditor = func;
	}

	void ContentBrowserPanel::CreateMaterialInstance(std::string& virtualPath)
	{
		//path is virtual path(material virtual path)
		//open dialog, to create material instance
		std::wstring physicalFilePath = FileDialogs::SaveFile(L"materialInstance(*.imat)\0*.imat\0");
		AssetManager::GetSingleton().AddMaterialInstanceToAssetRegistry(physicalFilePath);

		Ref<MaterialInstance> pMaterialInstance = CreateRef<MaterialInstance>();

		pMaterialInstance->SetMaterial(virtualPath);

		AssetManager::GetSingleton().CreateMaterialInstance(AssetManager::GetSingleton().to_string(physicalFilePath), pMaterialInstance);

		AssetManager::GetSingleton().AddMaterialInstanceToAssetRegistry(physicalFilePath);
	}

	void ContentBrowserPanel::RenderMaterialAssetPanel()
	{
		//check
		if(m_pSubMaterial->nextFrameNeedLoadTexture[0])
		{
			m_pSubMaterial->albedoMap = AssetManager::GetSingleton().GetTexture(m_pSubMaterial->albedoMapPath);
			m_pSubMaterial->nextFrameNeedLoadTexture[0] = false;
		}

		if (m_pSubMaterial->nextFrameNeedLoadTexture[1])
		{
			m_pSubMaterial->normalMap = AssetManager::GetSingleton().GetTexture(m_pSubMaterial->normalMapPath);
			m_pSubMaterial->nextFrameNeedLoadTexture[1] = false;
		}

		if (m_pSubMaterial->nextFrameNeedLoadTexture[2])
		{
			m_pSubMaterial->metallicMap = AssetManager::GetSingleton().GetTexture(m_pSubMaterial->metallicMapPath);
			m_pSubMaterial->nextFrameNeedLoadTexture[2] = false;
		}

		if (m_pSubMaterial->nextFrameNeedLoadTexture[3])
		{
			m_pSubMaterial->roughnessMap = AssetManager::GetSingleton().GetTexture(m_pSubMaterial->roughnessMapPath);
			m_pSubMaterial->nextFrameNeedLoadTexture[3] = false;
		}

		if (m_pSubMaterial->nextFrameNeedLoadTexture[4])
		{
			m_pSubMaterial->aoMap = AssetManager::GetSingleton().GetTexture(m_pSubMaterial->aoMapPath);
			m_pSubMaterial->nextFrameNeedLoadTexture[4] = false;
		}

		uint32_t DescriptorSize = Device::Get()->GetDescriptorAllocator((uint32_t)DescriptorHeapType::CBV_UAV_SRV)->GetDescriptorSize();

		std::vector<DescriptorHandle> handles;
		for(uint32_t i = 0; i < 5; ++i)
		{
			DescriptorHandle handle = (*m_MaterialAssetTextureHandle) + i * DescriptorSize;

			handles.push_back(handle);
		}

		Device::Get()->CopyDescriptorsSimple(1, handles[0].GetCpuHandle(), m_pSubMaterial->albedoMap->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, handles[1].GetCpuHandle(), m_pSubMaterial->normalMap->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, handles[2].GetCpuHandle(), m_pSubMaterial->metallicMap->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, handles[3].GetCpuHandle(), m_pSubMaterial->roughnessMap->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);
		Device::Get()->CopyDescriptorsSimple(1, handles[4].GetCpuHandle(), m_pSubMaterial->aoMap->GetCpuDescriptorHandle(), DescriptorHeapType::CBV_UAV_SRV);

		//render material asset panel
		ImGui::Begin("Material Asset Panel", &m_IsOpen);

		if(ImGui::Button("Save Button"))
		{
			//save the material asset
			AssetManager::GetSingleton().CreateSubMaterial(m_CurrentSubMaterialPath, m_pSubMaterial);
			AssetManager::GetSingleton().AddMaterialToAssetRegistry(AssetManager::GetSingleton().to_wsrting(m_CurrentSubMaterialPath));

			std::filesystem::path currentSubMaterialPath(m_CurrentSubMaterialPath);

			auto& relativePath = std::filesystem::relative(currentSubMaterialPath, g_AssetPath);

			//and if the asset manager have the same material, then to update that material
			AssetManager::GetSingleton().UpdateMaterial(relativePath.string(), m_pSubMaterial);
		}

		ImGui::Text("albedoMap");
		ImGui::Image(ImTextureID(handles[0].GetGpuHandle()->GetGpuPtr()), ImVec2(64.0f, 64.0f));
		if(ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				//check the texture is in asset manager's asset registry
				std::string textureVirtualPath = static_cast<const char*>(payload->Data);

				if(AssetManager::GetSingleton().GetTexture(textureVirtualPath) != nullptr)
				{
					m_pSubMaterial->albedoMapPath = textureVirtualPath;

					//next frame to load the texture
					m_pSubMaterial->nextFrameNeedLoadTexture[0] = true;
				}
			}
		}
		ImGui::ColorEdit3("Albedo", glm::value_ptr(m_pSubMaterial->gAlbedo));
		ImGui::Checkbox("IsTransParent", &m_pSubMaterial->IsTransparent);

		ImGui::Text("normalMap");
		ImGui::Image(ImTextureID(handles[1].GetGpuHandle()->GetGpuPtr()), ImVec2(64.0f, 64.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				//check the texture is in asset manager's asset registry
				std::string textureVirtualPath = static_cast<const char*>(payload->Data);

				if (AssetManager::GetSingleton().GetTexture(textureVirtualPath) != nullptr)
				{
					m_pSubMaterial->normalMapPath = textureVirtualPath;

					//next frame to load the texture
					m_pSubMaterial->nextFrameNeedLoadTexture[1] = true;
				}
			}
		}
		ImGui::ColorEdit3("Normal", glm::value_ptr(m_pSubMaterial->gNormal));
		ImGui::Checkbox("HaveNormal", &(m_pSubMaterial->HaveNormal));

		ImGui::Text("metallicMap");
		ImGui::Image(ImTextureID(handles[2].GetGpuHandle()->GetGpuPtr()), ImVec2(64.0f, 64.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				//check the texture is in asset manager's asset registry
				std::string textureVirtualPath = static_cast<const char*>(payload->Data);

				if (AssetManager::GetSingleton().GetTexture(textureVirtualPath) != nullptr)
				{
					m_pSubMaterial->metallicMapPath = textureVirtualPath;

					//next frame to load the texture
					m_pSubMaterial->nextFrameNeedLoadTexture[2] = true;
				}
			}
		}
		ImGui::DragFloat("Metallic", &(m_pSubMaterial->gMetallic), 0.05f, 0.0f, 1.0f);

		ImGui::Text("roughnessMap");
		ImGui::Image(ImTextureID(handles[3].GetGpuHandle()->GetGpuPtr()), ImVec2(64.0f, 64.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				//check the texture is in asset manager's asset registry
				std::string textureVirtualPath = static_cast<const char*>(payload->Data);

				if (AssetManager::GetSingleton().GetTexture(textureVirtualPath) != nullptr)
				{
					m_pSubMaterial->roughnessMapPath = textureVirtualPath;

					//next frame to load the texture
					m_pSubMaterial->nextFrameNeedLoadTexture[3] = true;
				}
			}
		}
		ImGui::DragFloat("Roughness", &(m_pSubMaterial->gRoughness), 0.05f, 0.0f, 1.0f);

		ImGui::Text("aoMap");
		ImGui::Image(ImTextureID(handles[4].GetGpuHandle()->GetGpuPtr()), ImVec2(64.0f, 64.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				//check the texture is in asset manager's asset registry
				std::string textureVirtualPath = static_cast<const char*>(payload->Data);

				if (AssetManager::GetSingleton().GetTexture(textureVirtualPath) != nullptr)
				{
					m_pSubMaterial->aoMapPath = textureVirtualPath;

					//next frame to load the texture
					m_pSubMaterial->nextFrameNeedLoadTexture[4] = true;
				}
			}
		}
		ImGui::DragFloat("Ao", &(m_pSubMaterial->gAo), 0.05f, 0.0f, 1.0f);
		ImGui::End();
	}
}
