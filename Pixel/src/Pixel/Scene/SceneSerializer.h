#pragma once

#include "Scene.h"

namespace Pixel {

	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		//------serializer------
		bool Serializer(const std::string& filePath);

		void WriteEntity(rapidjson::Writer<rapidjson::StringBuffer>& out, Entity& entity);

		//------write the variable name as key------
		void ToJsonRecursive(const rttr::instance& obj2, rapidjson::Writer<rapidjson::StringBuffer>& out);
		//------write the variable name as key------

		//------write the variable value------
		bool writeVariant(rttr::variant& var, rapidjson::Writer<rapidjson::StringBuffer>& out);
		//------write the variable value------

		bool WriteAtomicTypesToJson(const rttr::type& t, const rttr::variant& var, rapidjson::Writer<rapidjson::StringBuffer>& out);

		void WriteArray(const rttr::variant_sequential_view& view, rapidjson::Writer<rapidjson::StringBuffer>& out);
		//------serializer------

		//------deserializer------
		void Deserializer(const std::string& filePath);

		void ReadEntity(Entity& newEntity, const rapidjson::Value& object);

		void FromJsonRecursive(rttr::instance obj2, const rapidjson::Value& value);

		void ReadArrayRecursively(rttr::variant_sequential_view& view, const rapidjson::Value& value);

		rttr::variant ExtractBasicTypes(const rapidjson::Value& jsonValue);
		//------deserializer------
	private:
		Ref<Scene> m_Scene;
	};
}
