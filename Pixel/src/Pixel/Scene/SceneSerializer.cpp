#include "pxpch.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "Components.h"

#include <fstream>

namespace Pixel {

	static void WriteEntity(rapidjson::Writer<rapidjson::StringBuffer>& out, Entity& entity)
	{
		if(entity.HasComponent<IDComponent>())
		{
			//get the typedescriptor
			Reflect::TypeDescriptor* typeDesc = Reflect::TypeResolver<IDComponent>().get();
			out.Key(typeDesc->name);
			out.StartObject();
			typeDesc->Write(out, &entity.GetComponent<IDComponent>(), nullptr);
			out.EndObject();
		}
		if (entity.HasComponent<TagComponent>())
		{
			//get the typedescriptor
			Reflect::TypeDescriptor* typeDesc = Reflect::TypeResolver<TagComponent>().get();
			//TagComponent& tagComponent = entity.GetComponent<TagComponent>();
			out.Key(typeDesc->name);
			out.StartObject();
			typeDesc->Write(out, &entity.GetComponent<TagComponent>(), nullptr);
			out.EndObject();
		}
		if(entity.HasComponent<TransformComponent>())
		{
			Reflect::TypeDescriptor* typeDesc = Reflect::TypeResolver<TransformComponent>().get();
			out.Key(typeDesc->name);
			out.StartObject();
			typeDesc->Write(out, &entity.GetComponent<TransformComponent>(), nullptr);
			out.EndObject();
		}
		if(entity.HasComponent<CameraComponent>())
		{
			Reflect::TypeDescriptor* typeDesc = Reflect::TypeResolver<CameraComponent>().get();
			out.Key(typeDesc->name);
			out.StartObject();
			typeDesc->Write(out, &entity.GetComponent<CameraComponent>(), nullptr);
			out.EndObject();
		}
		if(entity.HasComponent<StaticMeshComponent>())
		{
			Reflect::TypeDescriptor* typeDesc = Reflect::TypeResolver<StaticMeshComponent>().get();
			out.Key(typeDesc->name);
			out.StartObject();
			typeDesc->Write(out, &entity.GetComponent<StaticMeshComponent>(), nullptr);
			out.EndObject();
		}
		if(entity.HasComponent<MaterialComponent>())
		{
			Reflect::TypeDescriptor* typeDesc = Reflect::TypeResolver<MaterialComponent>().get();
			out.Key(typeDesc->name);
			out.StartObject();
			typeDesc->Write(out, &entity.GetComponent<MaterialComponent>(), nullptr);
			out.EndObject();
		}
		if(entity.HasComponent<LightComponent>())
		{
			Reflect::TypeDescriptor* typeDesc = Reflect::TypeResolver<LightComponent>().get();
			out.Key(typeDesc->name);
			out.StartObject();
			typeDesc->Write(out, &entity.GetComponent<LightComponent>(), nullptr);
			out.EndObject();
		}
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
	{
		m_Scene = scene;
	}

	void SceneSerializer::Writer(const std::string& filepath)
	{
		rapidjson::StringBuffer strBuf;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strBuf);

		writer.StartObject();
		writer.Key("Entities");
		writer.StartArray();
		m_Scene->m_Registry.each([&](auto entityID)
		{
			writer.StartObject();
			Entity entity = { entityID, m_Scene.get() };
			if (!entity)
				return;
			
			//Serialize Entity
			WriteEntity(writer, entity);
			writer.EndObject();
		}
		);
		writer.EndArray();
		writer.EndObject();
		std::string data = strBuf.GetString();
		std::ofstream fout(filepath);
		fout << data.c_str();
		fout.close();
	}

	bool SceneSerializer::Read(const std::string& filepath)
	{
		//rapidjson::Value doc;
		rapidjson::Document doc;

		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		if(!doc.Parse(strStream.str().data()).HasParseError())
		{
			if(doc.HasMember("Entities") && doc["Entities"].IsArray())
			{
				const rapidjson::Value& entityArray = doc["Entities"];

				rapidjson::SizeType len = entityArray.Size();

				for(rapidjson::SizeType i = 0; i < len; ++i)
				{
					const rapidjson::Value& object = entityArray[i];

					Entity& newEntity = m_Scene->CreateEntityWithUUID(UUID(), "");

					//deserializer
					Reflect::TypeDescriptor* typeDesc = Reflect::TypeResolver<IDComponent>().get();
					if(object.HasMember(typeDesc->name))
					{
						typeDesc->Read(const_cast<rapidjson::Value&>(object[typeDesc->name]), &newEntity.GetComponent<IDComponent>(), nullptr);
					}

					typeDesc = Reflect::TypeResolver<TagComponent>().get();
					if(object.HasMember(typeDesc->name))
					{
						typeDesc->Read(const_cast<rapidjson::Value&>(object[typeDesc->name]), &newEntity.GetComponent<TagComponent>(), nullptr);
					}

					typeDesc = Reflect::TypeResolver<TransformComponent>().get();
					if(object.HasMember(typeDesc->name))
					{
						typeDesc->Read(const_cast<rapidjson::Value&>(object[typeDesc->name]), &newEntity.GetComponent<TransformComponent>(), nullptr);
					}

					typeDesc = Reflect::TypeResolver<CameraComponent>().get();
					if(object.HasMember(typeDesc->name))
					{
						auto& camera = newEntity.AddComponent<CameraComponent>();
						typeDesc->Read(const_cast<rapidjson::Value&>(object[typeDesc->name]), &camera, nullptr);
					}

					typeDesc = Reflect::TypeResolver<StaticMeshComponent>().get();
					if (object.HasMember(typeDesc->name))
					{
						auto& mesh = newEntity.AddComponent<StaticMeshComponent>();
						typeDesc->Read(const_cast<rapidjson::Value&>(object[typeDesc->name]), &mesh, nullptr);
						mesh.PostLoad();
					}

					typeDesc = Reflect::TypeResolver<MaterialComponent>().get();
					if (object.HasMember(typeDesc->name))
					{
						auto& material = newEntity.AddComponent<MaterialComponent>();
						typeDesc->Read(const_cast<rapidjson::Value&>(object[typeDesc->name]), &material, nullptr);
						material.PostLoad();
					}

					typeDesc = Reflect::TypeResolver<LightComponent>().get();
					if (object.HasMember(typeDesc->name))
					{
						auto& light = newEntity.AddComponent<LightComponent>();
						typeDesc->Read(const_cast<rapidjson::Value&>(object[typeDesc->name]), &light, nullptr);
					}
				}
			}
			stream.close();
		}
		else
		{
			stream.close();
			return false;
		}
		return true;
	}
}
