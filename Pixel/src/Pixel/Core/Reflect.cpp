#include "pxpch.h"

//------other library------
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <yaml-cpp/yaml.h>
//------other library------

#include "Reflect.h"

namespace Pixel {
	namespace Reflect {
		//------int------
		struct TypeDescriptor_Int : TypeDescriptor
		{
			TypeDescriptor_Int() : TypeDescriptor("int", sizeof(int))
			{}

			void Serializer(YAML::Emitter& out, void* obj) override
			{
				out << *(static_cast<int*>(obj));
			}

			void Deserializer(YAML::Node& node, void* obj, const char* name) override
			{
				//write to obj
				*(static_cast<int*>(obj)) = node[name].as<int>();
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
			TypeDescriptor_Uint64_t() : TypeDescriptor("int", sizeof(int))
			{}

			void Serializer(YAML::Emitter& out, void* obj) override
			{
				out << *(static_cast<uint64_t*>(obj));
			}

			void Deserializer(YAML::Node& node, void* obj, const char* name) override
			{
				//write to obj
				*(static_cast<uint64_t*>(obj)) = node[name].as<uint64_t>();
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

			void Serializer(YAML::Emitter& out, void* obj) override
			{
				out << *(static_cast<float*>(obj));
			}

			void Deserializer(YAML::Node& node, void* obj, const char* name) override
			{
				*(static_cast<float*>(obj)) = node[name].as<float>();
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

			void Serializer(YAML::Emitter& out, void* obj) override
			{
				out << *(static_cast<bool*>(obj));
			}

			void Deserializer(YAML::Node& node, void* obj, const char* name) override
			{
				*(static_cast<bool*>(obj)) = node[name].as<bool>();
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

			void Serializer(YAML::Emitter& out, void* obj) override
			{
				out << *(static_cast<std::string*>(obj));
			}

			void Deserializer(YAML::Node& node, void* obj, const char* name) override
			{
				*(static_cast<std::string*>(obj)) = node[name].as<std::string>();
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
		void TypeDescriptor_Struct::Serializer(YAML::Emitter& out, void* obj)
		{
			for (const Member& member : members)
			{
				out << YAML::Key;
				out << member.name;//variable name
				out << YAML::Value;
				member.type->Serializer(out, (char*)obj + member.offset);
			}
		}
		void TypeDescriptor_Struct::Deserializer(YAML::Node& node, void* obj, const char* name)
		{
			for(const Member& member : members)
			{
				member.type->Deserializer(node, (char*)obj + member.offset, member.name);
			}
		}
		//------struct/class------

		//------enumerate------
		struct TypeDescriptor_Enum : TypeDescriptor
		{
			TypeDescriptor_Enum() : TypeDescriptor("enumerate", sizeof(int32_t))
			{}

			void Serializer(YAML::Emitter& out, void* obj) override
			{
				out << *(static_cast<int*>(obj));
			}

			void Deserializer(YAML::Node& node, void* obj, const char* name) override
			{
				*(static_cast<int*>(obj)) = node[name].as<int>();
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

			virtual void Serializer(YAML::Emitter& out, void* obj) override
			{
				out << YAML::BeginSeq;
				size_t itemSize = getSize(obj);
				for(size_t i = 0; i < itemSize; ++i)
				{
					out << YAML::Key << itemType->name << YAML::Value;
					itemType->Serializer(out, const_cast<void*>(getItem(obj, i)));
				}
				out << YAML::EndSeq;
			}

			virtual void Deserializer(YAML::Node& node, void* obj, const char* name) override
			{
				//resize
				ReSize(obj, node[name].size());

				for(size_t i = 0; i < node[name].size(); ++i)
				{
					itemType->Deserializer(node[name][i], const_cast<void*>(getItem(obj, i)), itemType->name);
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