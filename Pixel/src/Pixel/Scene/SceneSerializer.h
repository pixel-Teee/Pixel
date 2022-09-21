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
		void ToJsonRecursive(const rttr::instance& obj2, rapidjson::Writer<rapidjson::StringBuffer>& out);
		//------write the variable name as key------

		//------write the variable value------
		bool writeVariant(rttr::variant& var, rapidjson::Writer<rapidjson::StringBuffer>& out);
		//------write the variable value------

		bool WriteAtomicTypesToJson(const rttr::type& t, const rttr::variant& var, rapidjson::Writer<rapidjson::StringBuffer>& out);

		void WriteArray(const rttr::variant_sequential_view& view, rapidjson::Writer<rapidjson::StringBuffer>& out);
		//------serializer------

		//------deserializer------

		bool Deserializer(const std::string& filePath);

		void ReadEntity(Entity& newEntity, rapidjson::Value& object);

		void FromJsonRecursive(rttr::instance obj2, rapidjson::Value& value);

		void ReadArrayRecursively(rttr::variant_sequential_view& view, rapidjson::Value& value);

		rttr::variant ExtractBasicTypes(const rapidjson::Value& jsonValue);
		//------deserializer------
	private:
		Ref<Scene> m_Scene;
	};
}
