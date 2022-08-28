#include "pxpch.h"

#include "Reflect.h"

//------other library------
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <yaml-cpp/yaml.h>

#include <rapidjson/stringbuffer.h>
//------other library------

namespace Pixel {
	namespace Reflect {
		//------int------
		struct TypeDescriptor_Int : TypeDescriptor
		{
			TypeDescriptor_Int() : TypeDescriptor("int", sizeof(int))
			{}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name, bool isArrayItem) override
			{
				if(!isArrayItem)
					writer.Key(name);
				writer.Int(*(static_cast<int*>(obj)));
			}

			virtual void Read(rapidjson::Value& value, void* obj, const char* name, bool isArrayItem) override
			{
				if (isArrayItem)
				{
					*static_cast<int*>(obj) = value.GetInt();
				}
				else if (value.HasMember(name) && value[name].IsInt())
				{
					*static_cast<int*>(obj) = value[name].GetInt();
				}
			}

		};

		template<>
		TypeDescriptor* getPrimitiveDescriptor<int>()
		{
			static TypeDescriptor_Int typeDesc;
			return &typeDesc;
		}
		//------int------

		//------int------
		struct TypeDescriptor_Uint32_t : TypeDescriptor
		{
			TypeDescriptor_Uint32_t() : TypeDescriptor("uint32_t", sizeof(uint32_t))
			{}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name, bool isArrayItem) override
			{
				if(!isArrayItem)
					writer.Key(name);
				writer.Int(*(static_cast<uint32_t*>(obj)));
			}

			virtual void Read(rapidjson::Value& value, void* obj, const char* name, bool isArrayItem) override
			{
				if (isArrayItem)
				{
					*static_cast<int*>(obj) = value.GetInt();
				}
				else if (value.HasMember(name) && value[name].IsInt())
				{
					*static_cast<int*>(obj) = value[name].GetInt();
				}
			}

		};

		template<>
		TypeDescriptor* getPrimitiveDescriptor<uint32_t>()
		{
			static TypeDescriptor_Uint32_t typeDesc;
			return &typeDesc;
		}
		//------int------

		//------uint64_t------
		struct TypeDescriptor_Uint64_t : TypeDescriptor
		{
			TypeDescriptor_Uint64_t() : TypeDescriptor("uint64_t", sizeof(uint64_t))
			{}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name, bool isArrayItem) override
			{
				if(!isArrayItem)
					writer.Key(name);
				writer.Uint64(*static_cast<uint64_t*>(obj));
			}

			virtual void Read(rapidjson::Value& value, void* obj, const char* name, bool isArrayItem) override
			{
				if (isArrayItem)
				{
					*static_cast<uint64_t*>(obj) = value.GetUint64();
				}
				else if (value.HasMember(name) && value[name].IsUint64())
				{
					*static_cast<uint64_t*>(obj) = value[name].GetUint64();
				}
			}
		};

		template<>
		TypeDescriptor* getPrimitiveDescriptor<uint64_t>()
		{
			static TypeDescriptor_Uint64_t typeDesc;
			return &typeDesc;
		}
		//------uint64_t------

		//------float------
		struct TypeDescriptor_Float : TypeDescriptor
		{
			TypeDescriptor_Float() : TypeDescriptor("float", sizeof(float))
			{}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name, bool isArrayItem) override
			{
				if(!isArrayItem)
					writer.Key(name);
				writer.Double(*static_cast<double*>(obj));
			}

			virtual void Read(rapidjson::Value& value, void* obj, const char* name, bool isArrayItem) override
			{
				if (isArrayItem)
				{
					*static_cast<double*>(obj) = value.GetDouble();
				}
				else if (value.HasMember(name) && value[name].IsFloat())
				{
					*static_cast<double*>(obj) = value[name].GetDouble();
				}
			}

		};

		template<>
		TypeDescriptor* getPrimitiveDescriptor<float>()
		{
			static TypeDescriptor_Float typeDesc;
			return &typeDesc;
		}
		//------float------

		//------Bool------
		struct TypeDescriptor_Bool : TypeDescriptor
		{
			TypeDescriptor_Bool() : TypeDescriptor("bool", sizeof(bool))
			{}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name, bool isArrayItem) override
			{
				if(!isArrayItem)
					writer.Key(name);
				writer.Bool(*static_cast<bool*>(obj));
			}

			virtual void Read(rapidjson::Value& value, void* obj, const char* name, bool isArrayItem) override
			{
				if (isArrayItem)
				{
					*static_cast<bool*>(obj) = value.GetBool();
				}
				else if (value.HasMember(name) && value[name].IsBool())
				{
					*static_cast<bool*>(obj) = value[name].GetBool();
				}
			}
		};

		template<>
		TypeDescriptor* getPrimitiveDescriptor<bool>()
		{
			static TypeDescriptor_Bool typeDesc;
			return &typeDesc;
		}
		//------Bool------

		//------String------
		struct TypeDescriptor_String : TypeDescriptor
		{
			TypeDescriptor_String() : TypeDescriptor("string", sizeof(std::string))
			{}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name, bool isArrayItem) override
			{
				if(!isArrayItem)
					writer.Key(name);
				//const std::string temp = *static_cast<std::string*>(obj) + '\0';
				std::string& str = *static_cast<std::string*>(obj);
				writer.String(str.c_str());
			}

			virtual void Read(rapidjson::Value& value, void* obj, const char* name, bool isArrayItem) override
			{
				if (isArrayItem)
				{
					*static_cast<std::string*>(obj) = value.GetString();
				}
				else if (value.HasMember(name) && value[name].IsString())
				{
					*static_cast<std::string*>(obj) = value[name].GetString();
				}
			}
		};

		template<>
		TypeDescriptor* getPrimitiveDescriptor<std::string>()
		{
			static TypeDescriptor_String typeDesc;
			return &typeDesc;
		}
		//------String------

		//------struct/class------
		void TypeDescriptor_Struct::Read(rapidjson::Value& value, void* obj, const char* name, bool isArrayItem)
		{
			if (isArrayItem)
			{
				for (const Member& member : members)
				{
					member.type->Read(value, (char*)obj + member.offset, member.name, false);
				}
			}
			else if (value.HasMember(name) && value[name].IsObject())
			{
				for (const Member& member : members)
				{
					member.type->Read(value[name], (char*)obj + member.offset, member.name, false);
				}
			}		
		}

		void TypeDescriptor_Struct::Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj,
			const char* name, bool isArrayItem)
		{
			if(!isArrayItem)
				writer.Key(name);
			writer.StartObject();
			for (const Member& member : members)
			{
				member.type->Write(writer, (char*)obj + member.offset, member.name, false);
			}
			writer.EndObject();
		}
		//------struct/class------

		//------enumerate------
		struct TypeDescriptor_Enum : TypeDescriptor
		{
			TypeDescriptor_Enum() : TypeDescriptor("enumerate", sizeof(int32_t))
			{}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name, bool isArrayItem) override
			{
				if(!isArrayItem)
					writer.Key(name);
				writer.Int(*static_cast<int*>(obj));
			}

			virtual void Read(rapidjson::Value& doc, void* obj, const char* name, bool isArrayItem) override
			{
				if (isArrayItem)
				{
					*static_cast<int*>(obj) = doc.GetInt();
				}
				else if (doc.HasMember(name) && doc[name].IsInt())
				{
					*static_cast<int*>(obj) = doc[name].GetInt();
				}
			}

		};

		TypeDescriptor* getEnumerateDescriptor()
		{
			static TypeDescriptor_Enum typeDesc;
			return &typeDesc;
		}
		//------enumerate------

		struct TypeDescriptor_GlmVec3 : TypeDescriptor
		{
			TypeDescriptor_GlmVec3() : TypeDescriptor("glm::vec3", sizeof(int32_t))
			{}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name, bool isArrayItem) override
			{
				if(!isArrayItem)
					writer.Key(name);
				glm::vec3& vec = *static_cast<glm::vec3*>(obj);
				writer.StartArray();
				writer.Double(vec.x);
				writer.Double(vec.y);
				writer.Double(vec.z);
				writer.EndArray();
			}

			virtual void Read(rapidjson::Value& doc, void* obj, const char* name, bool isArrayItem) override
			{
				if (isArrayItem)
				{
					glm::vec3& vec = *static_cast<glm::vec3*>(obj);
					vec.x = doc[0].GetDouble();
					vec.y = doc[1].GetDouble();
					vec.z = doc[2].GetDouble();
				}
				else if (doc.HasMember(name) && doc[name].IsArray())
				{
					glm::vec3& vec = *static_cast<glm::vec3*>(obj);
					vec.x = doc[name][0].GetDouble();
					vec.y = doc[name][1].GetDouble();
					vec.z = doc[name][2].GetDouble();
				}
			}

		};

		template<>
		TypeDescriptor* getPrimitiveDescriptor<glm::vec3>()
		{
			static TypeDescriptor_GlmVec3 typeDesc;
			return &typeDesc;
		}
	}	
}