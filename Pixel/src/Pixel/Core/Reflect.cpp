#include "pxpch.h"

//------other library------
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <yaml-cpp/yaml.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
//------other library------

#include "Reflect.h"

namespace Pixel {
	namespace Reflect {
		//------int------
		struct TypeDescriptor_Int : TypeDescriptor
		{
			TypeDescriptor_Int() : TypeDescriptor("int", sizeof(int))
			{}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name) override
			{
				writer.Key(name);
				writer.Int(*(static_cast<int*>(obj)));
			}

			virtual void Read(rapidjson::Document& doc, void* obj, const char* name) override
			{
				if (doc.HasMember(name) && doc[name].IsInt())
				{
					*static_cast<int*>(obj) = doc[name].GetInt();
				}
			}

			virtual void Read(rapidjson::Value& value, void* obj, const char* name) override
			{
				if (value.IsInt())
				{
					*static_cast<int*>(obj) = value.GetInt();
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

		//------uint64_t------
		struct TypeDescriptor_Uint64_t : TypeDescriptor
		{
			TypeDescriptor_Uint64_t() : TypeDescriptor("uint64_t", sizeof(uint64_t))
			{}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name) override
			{
				writer.Key(name);
				writer.Uint64(*static_cast<uint64_t*>(obj));
			}

			virtual void Read(rapidjson::Document& doc, void* obj, const char* name) override
			{
				if (doc.HasMember(name) && doc[name].IsUint64())
				{
					*static_cast<uint64_t*>(obj) = doc[name].GetUint64();
				}
			}
			virtual void Read(rapidjson::Value& value, void* obj, const char* name) override
			{
				if (value.IsInt())
				{
					*static_cast<uint64_t*>(obj) = value.GetUint64();
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

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name) override
			{
				writer.Key(name);
				writer.Double(*static_cast<double*>(obj));
			}

			virtual void Read(rapidjson::Document& doc, void* obj, const char* name) override
			{
				if (doc.HasMember(name) && doc[name].IsDouble())
				{
					*static_cast<double*>(obj) = doc[name].GetDouble();
				}
			}

			virtual void Read(rapidjson::Value& value, void* obj, const char* name) override
			{
				if (value.IsInt())
				{
					*static_cast<double*>(obj) = value.GetDouble();
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

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name) override
			{
				writer.Key(name);
				writer.Bool(*static_cast<bool*>(obj));
			}

			virtual void Read(rapidjson::Document& doc, void* obj, const char* name) override
			{
				if (doc.HasMember(name) && doc[name].IsBool())
				{
					*static_cast<bool*>(obj) = doc[name].GetBool();
				}
			}

			virtual void Read(rapidjson::Value& value, void* obj, const char* name) override
			{
				if (value.IsBool())
				{
					*static_cast<bool*>(obj) = value.GetBool();
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

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name) override
			{
				writer.Key(name);
				writer.String((*static_cast<std::string*>(obj)).data());
			}

			virtual void Read(rapidjson::Document& doc, void* obj, const char* name) override
			{
				if (doc.HasMember(name) && doc[name].IsString())
				{
					*static_cast<std::string*>(obj) = doc[name].GetString();
				}
			}

			virtual void Read(rapidjson::Value& value, void* obj, const char* name) override
			{
				if (value.IsString())
				{
					*static_cast<std::string*>(obj) = value.GetString();
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
		void TypeDescriptor_Struct::Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name)
		{
			for (const Member& member : members)
			{
				member.type->Write(writer, (char*)obj + member.offset, member.name);
			}
		}
		void TypeDescriptor_Struct::Read(rapidjson::Document& doc, void* obj, const char* name)
		{
			for (const Member& member : members)
			{
				member.type->Read(doc, (char*)obj + member.offset, member.name);
			}
		}
		void TypeDescriptor_Struct::Read(rapidjson::Value& value, void* obj, const char* name)
		{
			for (const Member& member : members)
			{
				member.type->Read(value, (char*)obj + member.offset, member.name);
			}
		}
		void TypeDescriptor_Struct::Read(rapidjson::Document& doc, void* obj, const char* name)
		{
			for (const Member& member : members)
			{
				member.type->Read(doc, (char*)obj + member.offset, name);
			}
		}
		//------struct/class------

		//------enumerate------
		struct TypeDescriptor_Enum : TypeDescriptor
		{
			TypeDescriptor_Enum() : TypeDescriptor("enumerate", sizeof(int32_t))
			{}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name) override
			{
				writer.Key(name);
				writer.Int(*static_cast<int*>(obj));
			}

			virtual void Read(rapidjson::Document& doc, void* obj, const char* name) override
			{
				if (doc.HasMember(name) && doc[name].IsInt())
				{
					*static_cast<int*>(obj) = doc[name].GetInt();
				}
			}

			virtual void Read(rapidjson::Value& value, void* obj, const char* name) override
			{
				if (value.IsInt())
				{
					*static_cast<int*>(obj) = value.GetInt();
				}
			}

		};

		TypeDescriptor* getEnumerateDescriptor()
		{
			static TypeDescriptor_Enum typeDesc;
			return &typeDesc;
		}
		//------enumerate------

		//------std::vector<>------
		struct TypeDescriptor_StdVector : TypeDescriptor
		{
			TypeDescriptor* itemType;
			size_t(*getSize)(const void*);
			const void* (*getItem)(const void*, size_t);
			void (*ReSize)(void*, size_t);

			template<typename ItemType>
			TypeDescriptor_StdVector(ItemType*):TypeDescriptor("std::vector<>", sizeof(std::vector<ItemType>)),
			itemType(TypeResolver<ItemType>::get())
			{
				getSize = [](const void* vecPtr)->size_t
				{
					const auto& vec = *(const std::vector<ItemType>*) vecPtr;
					return vec.size();
				};

				getItem = [](const void* vecPtr, size_t index)->const void*
				{
					const auto& vec = *(const std::vector<ItemType>*) vecPtr;
					return &vec[index];
				};

				ReSize = [](void* vecPtr, size_t size)->void
				{
					auto& vec = *(const std::vector<ItemType>*)vecPtr;
					vec.resize(size);
				};
			}

			virtual std::string getFullName() const override
			{
				return std::string("std::vector<") + itemType->getFullName() + ">";
			}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name) override
			{
				writer.Key(name);
				writer.StartArray();
				size_t vecSize = getSize(obj);
				for (size_t i = 0; i < vecSize; ++i)
				{
					itemType->Write(writer, const_cast<void*>(getItem(obj, i)), name);
				}
				writer.EndArray();
			}

			virtual void Read(rapidjson::Document& doc, void* obj, const char* name) override
			{
				if (doc.HasMember(name) && doc[name].IsArray())
				{
					rapidjson::Value& array = doc[name];
					for (size_t i = 0; i < array.Size(); ++i)
					{
						itemType->Read(array[i], const_cast<void*>(getItem(obj, i)), name);
					}
				}
			}

		};

		//partially specialize TypeResolver<> for std::vectors:
		template<typename T>
		class TypeResolver<std::vector<T>>
		{
		public:
			static TypeResolver* get()
			{
				static TypeDescriptor_StdVector typeDesc(static_cast<T*>(nullptr));
				return &typeDesc;
			}
		};
		//------std::vector<>------
	}	
}