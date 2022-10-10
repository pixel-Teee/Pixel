#pragma once

#include "Scene.h"

#include <rapidjson/writer.h>
#include <rapidjson/document.h>

namespace Pixel {

	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		//------serializer------
		bool Serializer(const std::string& filePath);

		void WriteEntity(rapidjson::Writer<rapidjson::StringBuffer>& out, Entity& entity);

		//------write the variable name as key------
		static void ToJsonRecursive(const rttr::instance& obj2, rapidjson::Writer<rapidjson::StringBuffer>& out, bool withType = false);
		//------write the variable name as key------

		//------write the variable value------
		static bool writeVariant(rttr::variant& var, rapidjson::Writer<rapidjson::StringBuffer>& out, bool withType);
		//------write the variable value------

		static bool WriteAtomicTypesToJson(const rttr::type& t, const rttr::variant& var, rapidjson::Writer<rapidjson::StringBuffer>& out);

		static void WriteArray(const rttr::variant_sequential_view& view, rapidjson::Writer<rapidjson::StringBuffer>& out, bool withType);
		//------serializer------

		//------deserializer------

		bool Deserializer(const std::string& filePath);

		void ReadEntity(Entity& newEntity, rapidjson::Value& object);

		static void FromJsonRecursive(rttr::instance obj2, rapidjson::Value& value, bool withType = false);

		static void ReadArrayRecursively(rttr::variant_sequential_view& view, rapidjson::Value& value, bool withType = false);

		static rttr::variant ExtractBasicTypes(const rapidjson::Value& jsonValue);
		//------deserializer------

		//------hash a struct------
		static size_t Hash(rttr::variant& var);
	private:
		Ref<Scene> m_Scene;
	};
}
