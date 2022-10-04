#include "pxpch.h"
#include "SceneSerializer.h"
#include "Entity.h"
#include "Components.h"

#include <fstream>

#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>

#include "Pixel/Renderer/3D/Material/ShaderMainFunction.h"
#include "Components/MaterialTreeComponent.h"

namespace Pixel {

	bool SceneSerializer::Serializer(const std::string& filePath)
	{
		rapidjson::StringBuffer strBuf;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strBuf);

		writer.StartObject();
		writer.Key("Entities");
		writer.StartArray();

		//iterate over the entity
		m_Scene->m_Registry.each([&](auto entityID)
		{
			writer.StartObject();
			Entity entity = { entityID, m_Scene.get() };
			if (!entity)
				return;

			//serialize entity
			WriteEntity(writer, entity);

			writer.EndObject();
		});

		writer.EndArray();
		writer.EndObject();
		std::string data = strBuf.GetString();
		std::ofstream fout(filePath);
		fout << data.c_str();
		fout.close();
		return true;
	}

	void SceneSerializer::WriteEntity(rapidjson::Writer<rapidjson::StringBuffer>& out, Entity& entity)
	{
		if (entity.HasComponent<IDComponent>())
		{
			//forward the entity's component reference to instance
			IDComponent obj = entity.GetComponent<IDComponent>();
			out.Key("IDComponent");
			//out.StartObject();
			ToJsonRecursive(obj, out, false);
			//out.EndObject();
		}
		if(entity.HasComponent<TagComponent>())
		{
			rttr::instance obj = entity.GetComponent<TagComponent>();
			out.Key("TagComponent");
			//out.StartObject();
			ToJsonRecursive(obj, out, false);
			//out.EndObject();
		}
		if (entity.HasComponent<TransformComponent>())
		{
			rttr::instance obj = entity.GetComponent<TransformComponent>();
			out.Key("TransformComponent");
			//out.StartObject();
			ToJsonRecursive(obj, out, false);
			//out.EndObject();
		}
		if (entity.HasComponent<CameraComponent>())
		{
			rttr::instance obj = entity.GetComponent<CameraComponent>();
			out.Key("CameraComponent");
			ToJsonRecursive(obj, out, false);
		}
		if (entity.HasComponent<StaticMeshComponent>())
		{
			rttr::instance obj = entity.GetComponent<StaticMeshComponent>();
			out.Key("StaticMeshComponent");
			ToJsonRecursive(obj, out, false);
		}
		if(entity.HasComponent<MaterialComponent>())
		{
			rttr::instance obj = entity.GetComponent<MaterialComponent>();
			out.Key("MaterialComponent");
			ToJsonRecursive(obj, out, false);
		}
		if (entity.HasComponent<LightComponent>())
		{
			rttr::instance obj = entity.GetComponent<LightComponent>();
			out.Key("LightComponent");
			ToJsonRecursive(obj, out, false);
		}
		if (entity.HasComponent<MaterialTreeComponent>())
		{
			rttr::instance obj = entity.GetComponent<MaterialTreeComponent>();
			out.Key("MaterialTreeComponent");
			ToJsonRecursive(obj, out, false);
		}
	}

	void SceneSerializer::ToJsonRecursive(const rttr::instance& obj2, rapidjson::Writer<rapidjson::StringBuffer>& out, bool withType)
	{
		out.StartObject();
		//dealing with wrapped objects
		rttr::instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;

		auto propList = obj.get_derived_type().get_properties();

		if (withType)
		{
			out.Key("type");
			out.String(obj.get_derived_type().get_raw_type().get_name().to_string().c_str());
		}

		//iterate over the property list, then write property
		for (auto prop : propList)
		{
			if (prop.get_metadata("NO_SERIALIZE"))
				continue;

			rttr::variant propValue = prop.get_value(obj);

			//PIXEL_CORE_INFO("{0}", propValue.get_type().get_name());

			if (!propValue)
				continue;//cannot serialize, because we cannot retrieve the value

			const auto name = prop.get_name();
			out.Key(name.to_string().c_str());
			if (!writeVariant(propValue, out, withType))
			{
				PIXEL_CORE_INFO("cannot serialize property, {0}", name);
			}
		}
		out.EndObject();
	}

	bool SceneSerializer::writeVariant(rttr::variant& var, rapidjson::Writer<rapidjson::StringBuffer>& out, bool withType)
	{
		auto valueType = var.get_type();
		auto wrappedType = valueType.is_wrapper() ? valueType.get_wrapped_type() : valueType;
		bool isWrapper = wrappedType != valueType;

		if (WriteAtomicTypesToJson(isWrapper ? wrappedType : valueType,
			isWrapper ? var.extract_wrapped_value() : var, out))
		{
			//...
		}
		else if (var.is_sequential_container())
		{
			WriteArray(var.create_sequential_view(), out, withType);
		}
		else
		{
			auto childProps = isWrapper ? wrappedType.get_properties() : valueType.get_properties();
			if (!childProps.empty())
			{
				ToJsonRecursive(var, out, withType);
			}
			else
			{
				bool ok = false;
				auto text = var.to_string(&ok);
				if (!ok)
				{
					out.String(text.c_str());
					return false;
				}

				out.String(text.c_str());
			}
		}

		return true;
	}

	void SceneSerializer::WriteArray(const rttr::variant_sequential_view& view, rapidjson::Writer<rapidjson::StringBuffer>& out, bool withType)
	{
		out.StartArray();

		for(const auto& item : view)
		{
			if(item.is_sequential_container())
			{
				WriteArray(item.create_sequential_view(), out, withType);
			}
			else
			{
				rttr::variant wrappedVar = item.extract_wrapped_value();
				rttr::type valueType = wrappedVar.get_type();
				if(valueType.is_arithmetic() || valueType == rttr::type::get<std::string>() || valueType.is_enumeration())
				{
					WriteAtomicTypesToJson(valueType, wrappedVar, out);
				}
				else//object
				{
					ToJsonRecursive(wrappedVar, out, withType);
				}
			}
		}

		out.EndArray();
	}

	bool SceneSerializer::WriteAtomicTypesToJson(const rttr::type& t, const rttr::variant& var,
		rapidjson::Writer<rapidjson::StringBuffer>& out)
	{
		if(t.is_arithmetic())
		{
			if (t == rttr::type::get<bool>())
				out.Bool(var.to_bool());
			else if (t == rttr::type::get<char>())
				out.Bool(var.to_bool());
			else if (t == rttr::type::get<int8_t>())
				out.Int(var.to_int8());
			else if (t == rttr::type::get<int16_t>())
				out.Int(var.to_int16());
			else if (t == rttr::type::get<int32_t>())
				out.Int(var.to_int32());
			else if (t == rttr::type::get<int64_t>())
				out.Int64(var.to_int64());
			else if (t == rttr::type::get<uint8_t>())
				out.Uint(var.to_uint8());
			else if (t == rttr::type::get<uint16_t>())
				out.Uint(var.to_uint16());
			else if (t == rttr::type::get<uint32_t>())
				out.Uint(var.to_uint32());
			else if (t == rttr::type::get<uint64_t>())
				out.Uint64(var.to_uint64());
			else if (t == rttr::type::get<float>())
				out.Double(var.to_double());
			else if (t == rttr::type::get<double>())
				out.Double(var.to_double());

			return true;
		}
		else if(t.is_enumeration())
		{
			bool ok = false;
			auto result = var.to_string(&ok);
			if(ok)
			{
				out.String(var.to_string().c_str());
			}
			else
			{
				ok = false;
				auto value = var.to_uint64(&ok);
				if (ok)
					out.Uint64(value);
				else
					out.Null();
			}

			return true;
		}
		else if(t == rttr::type::get<std::string>())
		{
			out.String(var.to_string().c_str());
			return true;
		}
		return false;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
	{
		m_Scene = scene;
	}

	/*
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
						typeDesc->Read(const_cast<rapidjson::Value&>(object[typeDesc->name]), &newEntity.GetComponent<IDComponent>(), nullptr, false);
					}

					typeDesc = Reflect::TypeResolver<TagComponent>().get();
					if(object.HasMember(typeDesc->name))
					{
						typeDesc->Read(const_cast<rapidjson::Value&>(object[typeDesc->name]), &newEntity.GetComponent<TagComponent>(), nullptr, false);
					}

					typeDesc = Reflect::TypeResolver<TransformComponent>().get();
					if(object.HasMember(typeDesc->name))
					{
						typeDesc->Read(const_cast<rapidjson::Value&>(object[typeDesc->name]), &newEntity.GetComponent<TransformComponent>(), nullptr, false);
					}

					typeDesc = Reflect::TypeResolver<CameraComponent>().get();
					if(object.HasMember(typeDesc->name))
					{
						auto& camera = newEntity.AddComponent<CameraComponent>();
						typeDesc->Read(const_cast<rapidjson::Value&>(object[typeDesc->name]), &camera, nullptr, false);
					}

					typeDesc = Reflect::TypeResolver<StaticMeshComponent>().get();
					if (object.HasMember(typeDesc->name))
					{
						auto& mesh = newEntity.AddComponent<StaticMeshComponent>();
						typeDesc->Read(const_cast<rapidjson::Value&>(object[typeDesc->name]), &mesh, nullptr, false);
						mesh.PostLoad();
					}

					typeDesc = Reflect::TypeResolver<MaterialComponent>().get();
					if (object.HasMember(typeDesc->name))
					{
						auto& material = newEntity.AddComponent<MaterialComponent>();
						typeDesc->Read(const_cast<rapidjson::Value&>(object[typeDesc->name]), &material, nullptr, false);
						material.PostLoad();
					}

					typeDesc = Reflect::TypeResolver<LightComponent>().get();
					if (object.HasMember(typeDesc->name))
					{
						auto& light = newEntity.AddComponent<LightComponent>();
						typeDesc->Read(const_cast<rapidjson::Value&>(object[typeDesc->name]), &light, nullptr, false);
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
	*/

	bool SceneSerializer::Deserializer(const std::string& filePath)
	{
		rapidjson::Document doc;

		std::ifstream stream(filePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		if (!doc.Parse(strStream.str().data()).HasParseError())
		{
			if (doc.HasMember("Entities") && doc["Entities"].IsArray())
			{
				rapidjson::Value& entityArray = doc["Entities"];

				rapidjson::SizeType len = entityArray.Size();

				for (rapidjson::SizeType i = 0; i < len; ++i)
				{
					rapidjson::Value& object = entityArray[i];

					Entity& newEntity = m_Scene->CreateEntityWithUUID(UUID(), "");

					ReadEntity(newEntity, object);
				}
			}
		}

		stream.close();

		return true;
	}

	void SceneSerializer::ReadEntity(Entity& newEntity, rapidjson::Value& object)
	{
		if (object.HasMember("IDComponent"))
		{
			rttr::instance obj = newEntity.GetComponent<IDComponent>();
			FromJsonRecursive(obj, object["IDComponent"], false);
		}
		if(object.HasMember("TagComponent"))
		{
			rttr::instance obj = newEntity.GetComponent<TagComponent>();
			FromJsonRecursive(obj, object["TagComponent"], false);
		}
		if(object.HasMember("TransformComponent"))
		{
			rttr::instance obj = newEntity.GetComponent<TransformComponent>();
			FromJsonRecursive(obj, object["TransformComponent"], false);
		}
		if(object.HasMember("CameraComponent"))
		{
			newEntity.AddComponent<CameraComponent>();
			rttr::instance obj = newEntity.GetComponent<CameraComponent>();
			FromJsonRecursive(obj, object["CameraComponent"], false);
		}
		if(object.HasMember("StaticMeshComponent"))
		{
			newEntity.AddComponent<StaticMeshComponent>();
			rttr::instance obj = newEntity.GetComponent<StaticMeshComponent>();
			FromJsonRecursive(obj, object["StaticMeshComponent"], false);
			newEntity.GetComponent<StaticMeshComponent>().PostLoad();
		}
		if (object.HasMember("MaterialComponent"))
		{
			newEntity.AddComponent<MaterialComponent>();
			rttr::instance obj = newEntity.GetComponent<MaterialComponent>();
			FromJsonRecursive(obj, object["MaterialComponent"], false);
			newEntity.GetComponent<MaterialComponent>().PostLoad();
		}
		if(object.HasMember("LightComponent"))
		{
			newEntity.AddComponent<LightComponent>();
			rttr::instance obj = newEntity.GetComponent<LightComponent>();
			FromJsonRecursive(obj, object["LightComponent"], false);
		}
		if (object.HasMember("MaterialTreeComponent"))
		{
			newEntity.AddComponent<MaterialTreeComponent>();
			rttr::instance obj = newEntity.GetComponent<MaterialTreeComponent>();
			FromJsonRecursive(obj, object["MaterialTreeComponent"], false);
		}
	}

	void SceneSerializer::FromJsonRecursive(rttr::instance obj2, rapidjson::Value& value, bool withType)
	{
		rttr::instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
		const auto propList = obj.get_derived_type().get_properties();
		
		for(auto prop : propList)
		{
			//std::cout << prop.get_name().to_string().c_str() << " " << prop.get_name().to_string().size() << std::endl;
			//variable name
			rapidjson::Value::MemberIterator ret = value.FindMember(prop.get_name().to_string().c_str());

			//if (value.HasMember(prop.get_name().to_string().c_str()))
			//	std::cout << "have member" << std::endl;

			if (ret == value.MemberEnd())
				continue;

			//value's type
			const rttr::type valueT = prop.get_type();

			auto& jsonValue = ret->value;

			//compare the jsonValue's type and valueType
			switch (jsonValue.GetType())
			{
				case rapidjson::kArrayType:
				{
					rttr::variant var;
					if (valueT.is_sequential_container())
					{
						var = prop.get_value(obj);
						auto view = var.create_sequential_view();
						//read
						ReadArrayRecursively(view, jsonValue, withType);
					}
					//if (prop.set_value(obj, var))
					//{
					//	std::cout << "create array!" << std::endl;
					//}
					prop.set_value(obj, var);
					break;
				}
				case rapidjson::kObjectType:
				{
					if (withType && valueT.get_wrapped_type().is_pointer())
					{
						rttr::type objType = rttr::type::get_by_name(jsonValue.FindMember("type")->value.GetString());
						//std::cout << objType.get_name().to_string().c_str() << std::endl;
						rttr::constructor ctor = objType.get_constructor();
						rttr::variant var = ctor.invoke();
						FromJsonRecursive(var, jsonValue, withType);
						prop.set_value(obj, var);
					}
					else
					{
						rttr::variant var = prop.get_value(obj);
						FromJsonRecursive(var, jsonValue, withType);
						prop.set_value(obj, var);
						break;
					}				
				}
				default:
				{
					rttr::variant extractedValue = ExtractBasicTypes(jsonValue);
					//std::cout << extractedValue << std::endl;
					if (extractedValue.convert(valueT))
						prop.set_value(obj, extractedValue);
				}
			}
		}
	}

	rttr::variant SceneSerializer::ExtractBasicTypes(const rapidjson::Value& jsonValue)
	{
		switch (jsonValue.GetType())
		{
		case rapidjson::kStringType:
		{
			return std::string(jsonValue.GetString());
			break;
		}
		case rapidjson::kNullType: break;
		case rapidjson::kFalseType:
		case rapidjson::kTrueType:
		{
			return jsonValue.GetBool();
			break;
		}
		case rapidjson::kNumberType:
		{
			if (jsonValue.IsInt())
				return jsonValue.GetInt();
			else if (jsonValue.IsDouble())
				return jsonValue.GetDouble();
			else if (jsonValue.IsUint())
				return jsonValue.GetUint();
			else if (jsonValue.IsInt64())
				return jsonValue.GetInt64();
			else if (jsonValue.IsUint64())
				return jsonValue.GetUint64();
			break;
		}
		case rapidjson::kObjectType:
		case rapidjson::kArrayType: return rttr::variant();
		}
		return rttr::variant();
	}

	void SceneSerializer::ReadArrayRecursively(rttr::variant_sequential_view& view, rapidjson::Value& value, bool withType)
	{
		view.set_size(value.Size());
		const rttr::type arrayValueType = view.get_rank_type(1);

		for (rapidjson::SizeType i = 0; i < value.Size(); ++i)
		{
			auto& jsonIndexValue = value[i];
			if (jsonIndexValue.IsArray())
			{
				auto subArrayView = view.get_value(i).create_sequential_view();
				ReadArrayRecursively(subArrayView, jsonIndexValue);
			}
			else if (jsonIndexValue.IsObject())
			{
				if (withType && arrayValueType.get_wrapped_type().is_pointer())
				{
					rttr::type objType = rttr::type::get_by_name(jsonIndexValue.FindMember("type")->value.GetString());
					rttr::constructor ctor = objType.get_constructor();
					rttr::variant var = ctor.invoke();//shared_ptr<ShaderMainFunction>
					//rttr::variant var2 = var.convert(arrayValueType);
					/*
					if (var.can_convert<Ref<ShaderMainFunction>>())
					{
						std::cout << "ShaderMainFunction" << std::endl;
					}
					*/

					//std::cout << wrappedVar.get_type().get_name() << std::endl;

					FromJsonRecursive(var, jsonIndexValue, withType);
					
					//std::cout << var.get_type().get_name() << std::endl;

					//std::cout << view.get_rank_type(1).get_name() << std::endl;

					//std::cout << arrayValueType.get_name().to_string().c_str() << std::endl;
					//std::cout << var.get_type().get_name().to_string().c_str() << std::endl;
					//std::cout << var.can_convert(arrayValueType);

					var.convert(arrayValueType);

					view.set_value(i, var);
				}
				else
				{
					rttr::variant varTmp = view.get_value(i);
					rttr::variant wrappedVar = varTmp.extract_wrapped_value();
					//ReadArrayRecursively(wrappedVar, jsonIndexValue);
					FromJsonRecursive(wrappedVar, jsonIndexValue, withType);
					view.set_value(i, wrappedVar);
				}				
			}
			else
			{
				rttr::variant extractedValue = ExtractBasicTypes(jsonIndexValue);
				if (extractedValue.convert(arrayValueType))
					view.set_value(i, extractedValue);
			}
		}
	}
}
