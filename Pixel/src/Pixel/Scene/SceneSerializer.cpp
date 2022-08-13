#include "pxpch.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "Components.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

//namespace YAML {
//	template<>
//	struct convert<glm::vec2>
//	{
//		static Node encode(const glm::vec2& rhs)
//		{
//			Node node;
//			node.push_back(rhs.x);
//			node.push_back(rhs.y);
//			node.SetStyle(EmitterStyle::Flow);
//			return node;
//		}
//
//		static bool decode(const Node& node, glm::vec2& rhs)
//		{
//			if (!node.IsSequence() || node.size() != 2)
//				return false;
//
//			rhs.x = node[0].as<float>();
//			rhs.y = node[1].as<float>();
//			return true;
//		}
//	};
//
//	template<>
//	struct convert<glm::vec3>
//	{
//		static Node encode(const glm::vec3& rhs)
//		{
//			Node node;
//			node.push_back(rhs.x);
//			node.push_back(rhs.y);
//			node.push_back(rhs.z);
//			node.SetStyle(EmitterStyle::Flow);
//			return node;
//		}
//
//		static bool decode(const Node& node, glm::vec3& rhs)
//		{
//			if (!node.IsSequence() || node.size() != 3)
//				return false;
//
//			rhs.x = node[0].as<float>();
//			rhs.y = node[1].as<float>();
//			rhs.z = node[2].as<float>();
//			return true;
//		}
//	};
//
//	template<>
//	struct convert<glm::vec4>
//	{
//		static Node encode(const glm::vec4& rhs)
//		{
//			Node node;
//			node.push_back(rhs.x);
//			node.push_back(rhs.y);
//			node.push_back(rhs.z);
//			node.push_back(rhs.w);
//			return node;
//		}
//
//		static bool decode(const Node& node, glm::vec4& rhs)
//		{
//			if (!node.IsSequence() || node.size() != 4)
//				return false;
//
//			rhs.x = node[0].as<float>();
//			rhs.y = node[1].as<float>();
//			rhs.z = node[2].as<float>();
//			rhs.w = node[3].as<float>();
//			return true;
//		}
//	};
//
//}

namespace Pixel {
	/*YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static std::string RigidBody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Pixel::Rigidbody2DComponent::BodyType::Static:    return "Static";
			case Pixel::Rigidbody2DComponent::BodyType::Dynamic:   return "Dynamic";
			case Pixel::Rigidbody2DComponent::BodyType::Kinematic: return "Kinematic";
		}
		PX_CORE_ASSERT(false, "Unknown BodyType");
		return {};
	}

	static Rigidbody2DComponent::BodyType RigidBody2DBodyTypeFromString(const std::string& bodyTypeString)
	{
		if(bodyTypeString == "Static") return Rigidbody2DComponent::BodyType::Static;
		if(bodyTypeString == "Dynamic") return Rigidbody2DComponent::BodyType::Dynamic;
		if(bodyTypeString == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;

		PX_CORE_ASSERT(false, "Unknown BodyType");
		return Rigidbody2DComponent::BodyType::Static;
	}

	static SceneCamera::ProjectionType ProjectionTypeFromInt(int projectionType)
	{
		if(projectionType == 0) return SceneCamera::ProjectionType::Perspective;
		if(projectionType == 1) return SceneCamera::ProjectionType::Orthographic;

		PX_CORE_ASSERT(false, "Unknown Camera Type");
		return SceneCamera::ProjectionType::Perspective;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
	:m_Scene(scene)
	{

	}*/

	//static void SerializeMaterialTreeComponent(MaterialTreeComponent& materialTreeComponent)
	//{
	//	//YAML::Emitter out;
	//	//Ref<Material> pMaterial = materialTreeComponent.m_pMaterial;
	//	//std::vector<Ref<ShaderFunction>> pShaderFunctions = pMaterial->GetShaderFunction();
	//	//
	//	//out << YAML::BeginMap;
	//	//for (uint32_t i = 0; i < pShaderFunctions.size(); ++i)
	//	//{
	//	//	//traverse every node, and output every node's input node and output node in location
	//	//	out << YAML::Key << "NodeName" << YAML::Value << pShaderFunctions[i]->GetShowName();
	//	//	out << YAML::Key << "NodeType" << YAML::Value << pShaderFunctions[i]->GetFunctionType();
	//	//	out << YAML::Key << "NodePos" << YAML::Value << pShaderFunctions[i]->GetFunctioNodePos();

	//	//	out << YAML::Key << "InputNode";
	//	//	out << YAML::Flow;
	//	//	out << YAML::Value;
	//	//	out << YAML::BeginSeq;
	//	//	for (uint32_t j = 0; j < pShaderFunctions[i]->GetInputNodeNum(); ++j)
	//	//	{
	//	//		out << pShaderFunctions[i]->GetInputNode(j)->GetOutputLink()->GetOwner()->GetShowName();
	//	//	}
	//	//	out << YAML::EndSeq;

	//	//	out << YAML::Key << "OutputNode";
	//	//	out << YAML::Flow;
	//	//	out << YAML::Value;
	//	//	out << YAML::BeginSeq;
	//	//	for (uint32_t j = 0; j < pShaderFunctions[i]->GetOutputNodeNum(); ++j)
	//	//	{
	//	//		//out << pShaderFunctions[i]->GetOutputNode(j)->
	//	//		out << pShaderFunctions[i]->GetOutputNode(j)->GetInputLink()[0]->GetOwner()->GetShowName();
	//	//	}
	//	//	out << YAML::EndSeq;
	//	//}
	//	//out << YAML::EndMap;
	//	////save the node relationship in one file
	//	//std::ofstream fstream;
	//	//fstream.open(materialTreeComponent.path);

	//	//fstream.close();
	//}

	//static void SerializeEntity(YAML::Emitter& out, Entity entity)
	//{
	//	out << YAML::BeginMap;//Entity
	//	out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();	

	//	if (entity.HasComponent<TagComponent>())
	//	{
	//		out << YAML::Key << "TagComponent";
	//		out << YAML::BeginMap; // TagComponent

	//		auto& tag = entity.GetComponent<TagComponent>().Tag;
	//		out << YAML::Key << "Tag" << YAML::Value << tag;

	//		out << YAML::EndMap; // TagComponent
	//	}

	//	if (entity.HasComponent<TransformComponent>())
	//	{
	//		out << YAML::Key << "TransformComponent";
	//		out << YAML::BeginMap; // TransformComponent

	//		auto& tc = entity.GetComponent<TransformComponent>();
	//		out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
	//		out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
	//		out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

	//		out << YAML::EndMap; // TransformComponent
	//	}

	//	if (entity.HasComponent<CameraComponent>())
	//	{
	//		out << YAML::Key << "CameraComponent";
	//		out << YAML::BeginMap; // CameraComponent

	//		auto& cameraComponent = entity.GetComponent<CameraComponent>();
	//		auto& camera = cameraComponent.camera;

	//		out << YAML::Key << "Camera" << YAML::Value;
	//		out << YAML::BeginMap; // Camera
	//		out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
	//		out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
	//		out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
	//		out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
	//		out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
	//		out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
	//		out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
	//		out << YAML::EndMap; // Camera

	//		out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
	//		out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

	//		out << YAML::EndMap; // CameraComponent
	//	}

	//	if (entity.HasComponent<SpriteRendererComponent>())
	//	{
	//		out << YAML::Key << "SpriteRendererComponent";
	//		out << YAML::BeginMap; // SpriteRendererComponent

	//		auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
	//		out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;

	//		out << YAML::EndMap; // SpriteRendererComponent
	//	}

	//	if (entity.HasComponent<Rigidbody2DComponent>())
	//	{
	//		out << YAML::Key << "Rigidbody2DComponent";
	//		out << YAML::BeginMap;

	//		auto& rb2dComponent = entity.GetComponent<Rigidbody2DComponent>();
	//		out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2dComponent.Type);
	//		out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.FixedRotation;

	//		out << YAML::EndMap;
	//	}

	//	if (entity.HasComponent<BoxCollider2DComponent>())
	//	{
	//		out << YAML::Key << "BoxCollider2DComponent";
	//		out << YAML::BeginMap;

	//		auto& bc2dComponent = entity.GetComponent<BoxCollider2DComponent>();
	//		out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.Offset;
	//		out << YAML::Key << "Size" << YAML::Value << bc2dComponent.Size;
	//		out << YAML::Key << "Density" << YAML::Value << bc2dComponent.Density;
	//		out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.Friction;
	//		out << YAML::Key << "Restitution" << YAML::Value << bc2dComponent.Restitution;
	//		out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dComponent.RestitutionThreshold;

	//		out << YAML::EndMap;
	//	}

	//	if (entity.HasComponent<StaticMeshComponent>())
	//	{
	//		out << YAML::Key << "StaticMeshComponent";
	//		out << YAML::BeginMap;

	//		auto& staticMeshComponent = entity.GetComponent<StaticMeshComponent>();
	//		out << YAML::Key << "Path" << YAML::Value << staticMeshComponent.path;

	//		out << YAML::EndMap;
	//	}

	//	if (entity.HasComponent<LightComponent>())
	//	{
	//		out << YAML::Key << "LightComponent";
	//		out << YAML::BeginMap;

	//		auto& lightComponent = entity.GetComponent<LightComponent>();

	//		uint32_t lightType = (uint32_t)lightComponent.lightType;

	//		out << YAML::Key << "LightType" << YAML::Value << lightType;
	//		
	//		out << YAML::Key << "Color" << YAML::Value << lightComponent.color;
	//		if (lightComponent.lightType == LightType::PointLight)
	//		{
	//			out << YAML::Key << "Constant" << YAML::Value << lightComponent.constant;
	//			out << YAML::Key << "Linear" << YAML::Value << lightComponent.linear;
	//			out << YAML::Key << "Quadratic" << YAML::Value << lightComponent.quadratic;
	//		}

	//		out << YAML::EndMap;
	//	}

	//	if (entity.HasComponent<MaterialComponent>())
	//	{
	//		out << YAML::Key << "MaterialComponent";
	//		out << YAML::BeginMap;
	//		
	//		auto& materialComponent = entity.GetComponent<MaterialComponent>();
	//		out << YAML::Key << "albedoPath" << YAML::Value << materialComponent.albedoPath;
	//		out << YAML::Key << "normalMapPath" << YAML::Value << materialComponent.normalMapPath;
	//		out << YAML::Key << "roughnessPath" << YAML::Value << materialComponent.roughnessPath;
	//		out << YAML::Key << "emissivePath" << YAML::Value << materialComponent.emissivePath;
	//		out << YAML::Key << "metallicPath" << YAML::Value << materialComponent.metallicPath;
	//		out << YAML::Key << "gAlbedo" << YAML::Value << materialComponent.gAlbedo;
	//		out << YAML::Key << "gNormal" << YAML::Value << materialComponent.gNormal;
	//		out << YAML::Key << "gRoughness" << YAML::Value << materialComponent.gRoughness;
	//		out << YAML::Key << "gMetallic" << YAML::Value << materialComponent.gMetallic;
	//		out << YAML::Key << "gEmissive" << YAML::Value << materialComponent.gEmissive;
	//		out << YAML::Key << "HaveNormal" << YAML::Value << materialComponent.HaveNormal;
	//		out << YAML::EndMap;
	//	}

	//	if (entity.HasComponent<MaterialTreeComponent>())
	//	{
	//		out << YAML::Key << "MaterialTreeComponent";
	//		out << YAML::BeginMap;

	//		auto& materialTreeComponent = entity.GetComponent<MaterialTreeComponent>();
	//		out << YAML::Key << "Path" << YAML::Value << materialTreeComponent.path;
	//		out << YAML::EndMap;
	//		//save the node relationship to materialTreeComponent.path's file;
	//		//SerializeMaterialTreeComponent(materialTreeComponent);
	//	}

	//	if (entity.HasComponent<NativeScriptComponent>())
	//	{
	//		out << YAML::Key << "ScriptComponent";
	//		out << YAML::BeginMap;

	//		auto& ScriptComponent = entity.GetComponent<NativeScriptComponent>();
	//		out << YAML::Key << "Path" << YAML::Value << ScriptComponent.m_path;
	//		out << YAML::EndMap;
	//	}

	//	if (entity.HasComponent<RigidBody3DComponent>())
	//	{
	//		out << YAML::Key << "RigidBody3DComponent";
	//		out << YAML::BeginMap;

	//		auto& rdg = entity.GetComponent<RigidBody3DComponent>();
	//		out << YAML::Key << "ShapeType" << YAML::Value << rdg.m_shapeType;
	//		out << YAML::Key << "BodyMass" << YAML::Value << rdg.m_bodyMass;
	//		out << YAML::Key << "BodyInertia" << YAML::Value << rdg.m_bodyInertia;
	//		out << YAML::Key << "Restitution" << YAML::Value << rdg.m_restitution;
	//		out << YAML::Key << "Friction" << YAML::Value << rdg.m_friction;

	//		if (rdg.m_shapeType == RigidBody3DComponent::ShapeType::BoxShape)
	//		{
	//			out << YAML::Key << "HalfLength" << YAML::Value << rdg.m_HalfLength;
	//		}
	//		else if (rdg.m_shapeType == RigidBody3DComponent::ShapeType::SphereShape)
	//		{
	//			out << YAML::Key << "Radius" << YAML::Value << rdg.m_Radius;
	//		}
	//		out << YAML::EndMap;
	//	}

	//	out << YAML::EndMap;
	//}

	//static void SerializeEnvironment(YAML::Emitter& out, const std::vector<std::string>& paths)
	//{
	//	out << YAML::BeginMap;
	//	out << YAML::Key << "EnvironmentType" << YAML::Value << "SkyBox";
	//	out << YAML::Key << "SkyBox";
	//	out << YAML::BeginMap;
	//	static std::vector<std::string> faces = {
	//		"Right",
	//		"Left",
	//		"Top",
	//		"Bottom",
	//		"Front",
	//		"Back"
	//	};

	//	for (uint32_t i = 0; i < paths.size(); ++i)
	//	{
	//		out << YAML::Key << faces[i] << YAML::Value << paths[i];
	//	}
	//	out << YAML::EndMap;
	//	out << YAML::EndMap;
	//}

	//void SceneSerializer::Serialize(const std::string& filepath)
	//{
	//	YAML::Emitter out;
	//	out << YAML::BeginMap;
	//	out << YAML::Key << "Scene" << YAML::Value << "Untitled";
	//	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
	//	m_Scene->m_Registry.each([&](auto entityID)
	//	{
	//		Entity entity = {entityID, m_Scene.get()};
	//		if(!entity)
	//			return;
	//		//Serialize Entity
	//		SerializeEntity(out, entity);
	//	}
	//	);
	//	out << YAML::EndSeq;

	//	//out << YAML::Key << "Environment" << YAML::BeginSeq;
	//	//SerializeEnvironment(out, m_Scene->m_skyBox->GetPaths());
	//	//out << YAML::EndSeq;

	//	out << YAML::EndMap;

	//	std::ofstream fout(filepath);
	//	fout << out.c_str();
	//}

	//void SceneSerializer::SerializeRuntime(const std::string& filepath)
	//{

	//}

	//bool SceneSerializer::Deserialize(const std::string& filepath)
	//{
	//	std::ifstream stream(filepath);
	//	std::stringstream strStream;
	//	strStream << stream.rdbuf();

	//	YAML::Node data = YAML::Load(strStream.str());
	//	if (!data["Scene"])
	//		return false;

	//	std::string sceneName = data["Scene"].as<std::string>();
	//	PIXEL_CORE_TRACE("Deserializing scene '{0}'", sceneName);

	//	auto entities = data["Entities"];
	//	if (entities)
	//	{
	//		for (auto entity : entities)
	//		{
	//		uint64_t uuid = entity["Entity"].as<uint64_t>();

	//		std::string name;
	//		auto tagComponent = entity["TagComponent"];
	//		if (tagComponent)
	//			name = tagComponent["Tag"].as<std::string>();

	//		PIXEL_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

	//		Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

	//		auto transformComponent = entity["TransformComponent"];
	//		if (transformComponent)
	//		{
	//			// Entities always have transforms
	//			auto& tc = deserializedEntity.GetComponent<TransformComponent>();
	//			tc.Translation = transformComponent["Translation"].as<glm::vec3>();
	//			tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
	//			tc.Scale = transformComponent["Scale"].as<glm::vec3>();
	//		}

	//		auto cameraComponent = entity["CameraComponent"];
	//		if (cameraComponent)
	//		{
	//			auto& cc = deserializedEntity.AddComponent<CameraComponent>();

	//			auto& cameraProps = cameraComponent["Camera"];
	//			//PIXEL_CORE_INFO(cameraProps["ProjectionType"].as<int>());
	//			cc.camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
	//			cc.camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
	//			cc.camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

	//			cc.camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
	//			cc.camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
	//			cc.camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

	//			cc.Primary = cameraComponent["Primary"].as<bool>();
	//			cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();

	//			cc.camera.SetProjectionType(ProjectionTypeFromInt(cameraProps["ProjectionType"].as<int>()));
	//		}

	//		auto spriteRendererComponent = entity["SpriteRendererComponent"];
	//		if (spriteRendererComponent)
	//		{
	//			auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
	//			src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
	//		}

	//		auto rigidBody2DComponent = entity["Rigidbody2DComponent"];
	//		if (rigidBody2DComponent)
	//		{
	//			auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
	//			rb2d.Type = RigidBody2DBodyTypeFromString(rigidBody2DComponent["BodyType"].as<std::string>());
	//			rb2d.FixedRotation = rigidBody2DComponent["FixedRotation"].as<bool>();
	//		}

	//		auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
	//		if (boxCollider2DComponent)
	//		{
	//			auto& bc2d = deserializedEntity.AddComponent<BoxCollider2DComponent>();
	//			bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
	//			bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
	//			bc2d.Density = boxCollider2DComponent["Density"].as<float>();
	//			bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
	//			bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
	//			bc2d.Restitution = boxCollider2DComponent["RestitutionThreshold"].as<float>();
	//		}

	//		auto staticMeshComponent = entity["StaticMeshComponent"];
	//		if (staticMeshComponent)
	//		{
	//			std::string str = staticMeshComponent["Path"].as<std::string>();
	//			auto& src = deserializedEntity.AddComponent<StaticMeshComponent>(str);
	//		}

	//		auto lightComponent = entity["LightComponent"];
	//		if (lightComponent)
	//		{
	//			auto& light = deserializedEntity.AddComponent<LightComponent>();

	//			uint32_t lightType = lightComponent["LightType"].as<int32_t>();
	//			if (lightType == 0) light.lightType = LightType::PointLight;
	//			else if (lightType == 1) light.lightType == LightType::DirectLight;
	//			else light.lightType = LightType::SpotLight;

	//			light.color = lightComponent["Color"].as<glm::vec3>();
	//			if (lightType == 0)
	//			{
	//				light.constant = lightComponent["Constant"].as<float>();
	//				light.linear = lightComponent["Linear"].as<float>();
	//				light.quadratic = lightComponent["Quadratic"].as<float>();
	//			}			
	//		}

	//		auto materialComponent = entity["MaterialComponent"];
	//		if (materialComponent)
	//		{
	//			std::string albedoPath = materialComponent["albedoPath"].as<std::string>();
	//			std::string roughnessPath = materialComponent["roughnessPath"].as<std::string>();
	//			std::string metallicPath = materialComponent["metallicPath"].as<std::string>();
	//			std::string normalMapPath = materialComponent["normalMapPath"].as<std::string>();
	//			std::string emissivePath = materialComponent["emissivePath"].as<std::string>();
	//			glm::vec3 albedo = materialComponent["gAlbedo"].as<glm::vec3>();
	//			glm::vec3 normal = materialComponent["gNormal"].as<glm::vec3>();
	//			float roughness = materialComponent["gRoughness"].as<float>();
	//			float metallic = materialComponent["gMetallic"].as<float>();
	//			float emissive = materialComponent["gEmissive"].as<float>();
	//			bool HaveNormal = materialComponent["HaveNormal"].as<bool>();
	//			auto& material = deserializedEntity.AddComponent<MaterialComponent>(albedoPath, normalMapPath, roughnessPath, metallicPath, emissivePath,
	//				albedo, normal, roughness, metallic, emissive, HaveNormal);
	//		}

	//		auto materialTreeComponent = entity["MaterialTreeComponent"];
	//		if (materialTreeComponent)
	//		{
	//			std::string Path = materialComponent["Path"].as<std::string>();
	//			auto& materialTree = deserializedEntity.AddComponent<MaterialTreeComponent>(Path);
	//		}

	//		auto scriptComponent = entity["ScriptComponent"];
	//		if (scriptComponent)
	//		{
	//			std::string Path = scriptComponent["Path"].as<std::string>();
	//			auto& script = deserializedEntity.AddComponent<NativeScriptComponent>();
	//			script.m_path = Path;
	//		}

	//		auto rigid3DComponent = entity["RigidBody3DComponent"];
	//		if (rigid3DComponent)
	//		{
	//			auto& rgd = deserializedEntity.AddComponent<RigidBody3DComponent>();
	//			rgd.m_shapeType = RigidBody3DComponent::ShapeType(rigid3DComponent["ShapeType"].as<int>());
	//			if (rigid3DComponent["ShapeType"].as<int32_t>() == RigidBody3DComponent::ShapeType::BoxShape)
	//			{
	//				rgd.m_HalfLength = rigid3DComponent["HalfLength"].as<float>();
	//			}
	//			else if (rigid3DComponent["ShapeType"].as<int32_t>() == RigidBody3DComponent::ShapeType::SphereShape)
	//			{
	//				rgd.m_Radius = rigid3DComponent["Radius"].as<float>();
	//			}
	//			rgd.m_bodyMass = rigid3DComponent["BodyMass"].as<float>();
	//			rgd.m_bodyInertia = rigid3DComponent["BodyInertia"].as<glm::vec3>();
	//			rgd.m_friction = rigid3DComponent["Friction"].as<float>();
	//			rgd.m_restitution = rigid3DComponent["Restitution"].as<float>();
	//		}
	//		}
	//	}

		//environment
		//{
		//	auto environments = data["Environment"];
		//	if (environments)
		//	{
		//		for (auto environment : environments)
		//		{
		//			std::string environmentType = environment["EnvironmentType"].as<std::string>();
		//			PX_CORE_ASSERT("environment type{0}", environmentType);
		//			auto SkyBox = environment["SkyBox"];
		//			if (SkyBox)
		//			{

		//				//PIXEL_CORE_TRACE("Deserializing environment '{0}'", environment);

		//				static std::vector<std::string> faces = {
		//					"Right",
		//					"Left",
		//					"Top",
		//					"Bottom",
		//					"Front",
		//					"Back"
		//				};

		//				std::vector<std::string> paths(6);

		//				for (uint32_t i = 0; i < 6; ++i)
		//				{
		//					paths[i] = SkyBox[faces[i]].as<std::string>();
		//				}
		//				
		//				for (uint32_t i = 0; i < 6; ++i)
		//				{
		//					m_Scene->m_skyBox->SetFace((FaceTarget)i, paths[i]);
		//				}
		//				m_Scene->m_skyBox->GenerateCubeMap();
		//				m_Scene->m_skyBox->SetDirty(true);
		//			}
		//		}
		//	}	
		//}	
	//	return true;
	//}

	//bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	//{
	//	//have not implemented
	//	//PX_CORE_ASSERT(false);

	//	return false;
	//}

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
