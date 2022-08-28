#pragma once

#include <string>
#include <vector>
#include <memory>
#include <type_traits>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>

namespace Pixel {
	namespace Reflect
	{
		struct TypeDescriptor {
			const char* name;
			size_t size;

			TypeDescriptor(const char* name, size_t size) : name{ name }, size{ size } {}
			virtual ~TypeDescriptor() {}
			virtual std::string getFullName() const { return name; }

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name, bool isArrayItem) = 0;//write to json
			virtual void Read(rapidjson::Value& value, void* obj, const char* name, bool isArrayItem) = 0;
		};
		//------finding type descriptors------

		//------declare the function template that handles primitive types such as int, std::string, etc.------
		template<typename T>
		TypeDescriptor* getPrimitiveDescriptor();

		TypeDescriptor* getEnumerateDescriptor();

		//------a helper class to find TypeDescriptors in different ways:------
		struct DefaultResolver
		{
			template<typename T> static char func(decltype(&T::Reflection));
			template<typename T> static int func(...);
			template<typename T>
			struct IsReflected
			{
				static constexpr int value = sizeof(func<T>(nullptr)) == sizeof(char);
			};

			//------this version is called if T has a static member named "Reflection"------
			template<typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0, typename std::enable_if<!std::is_enum<T>::value, int>::type = 0>
			static TypeDescriptor* get()
			{
				return &T::Reflection;
			}

			//------this version is called otherwise------
			template<typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0, typename std::enable_if<!std::is_enum<T>::value, int>::type = 0>
			static TypeDescriptor* get()
			{
				return getPrimitiveDescriptor<T>();
			}

			template<typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0, typename std::enable_if<std::is_enum<T>::value, int>::type = 0>
			static TypeDescriptor* get()
			{
				return getEnumerateDescriptor();
			}
		};

		//------this is primary class template for finding all TypeDescriptors------
		template<typename T>
		struct TypeResolver
		{
			static TypeDescriptor* get()
			{
				return DefaultResolver::get<T>();
			}
		};

		//------type descriptors for user-defined structs/classes------
		struct TypeDescriptor_Struct : TypeDescriptor {
			struct Member {
				const char* name;
				size_t offset;
				TypeDescriptor* type;
			};

			std::vector<Member> members;

			TypeDescriptor_Struct(void (*init)(TypeDescriptor_Struct*)) : TypeDescriptor{ nullptr, 0 } {
				init(this);
			}
			TypeDescriptor_Struct(const char* name, size_t size, const std::initializer_list<Member>& init) : TypeDescriptor{ nullptr, 0 }, members{ init } {
			}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name, bool isArrayItem) override;
			virtual void Read(rapidjson::Value& value, void* obj, const char* name, bool isArrayItem) override;
		};

		//------std::vector<>------
		struct TypeDescriptor_StdVector : TypeDescriptor
		{
			TypeDescriptor* itemType;
			size_t(*getSize)(const void*);
			const void* (*getItem)(const void*, size_t);
			void (*ReSize)(void*, size_t);

			template<typename ItemType>
			TypeDescriptor_StdVector(ItemType*) :TypeDescriptor("std::vector<>", sizeof(std::vector<ItemType>)),
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
					auto& vec = *(std::vector<ItemType>*)vecPtr;
					vec.resize(size);
				};
			}

			virtual std::string getFullName() const override
			{
				return std::string("std::vector<") + itemType->getFullName() + ">";
			}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name, bool isArrayItem) override
			{
				if(!isArrayItem)
					writer.Key(name);
				writer.StartArray();
				size_t vecSize = getSize(obj);
				//writer.StartObject();
				for (size_t i = 0; i < vecSize; ++i)
				{
					itemType->Write(writer, const_cast<void*>(getItem(obj, i)), itemType->getFullName().c_str(), true);
				}
				//writer.EndObject();
				writer.EndArray();
			}

			virtual void Read(rapidjson::Value& doc, void* obj, const char* name, bool isArrayItem) override
			{
				if (isArrayItem)
				{
					rapidjson::Value& array = doc;

					ReSize(obj, array.Size());

					for (rapidjson::SizeType i = 0; i < array.Size(); ++i)
					{
						itemType->Read(array[i], const_cast<void*>(getItem(obj, i)), nullptr, true);
					}
				}
				else if (doc.HasMember(name) && doc[name].IsArray())
				{
					rapidjson::Value& array = doc[name];

					ReSize(obj, array.Size());

					for (rapidjson::SizeType i = 0; i < array.Size(); ++i)
					{
						itemType->Read(array[i], const_cast<void*>(getItem(obj, i)), nullptr, true);
					}
				}
			}

		};

		//partially specialize TypeResolver<> for std::vectors:
		template<typename T>
		struct TypeResolver<std::vector<T>>
		{
			static TypeDescriptor* get()
			{
				static TypeDescriptor_StdVector typeDesc(static_cast<T*>(nullptr));
				return &typeDesc;
			}
		};
		//------std::vector<>------

		//------std::shared_ptr<>------
		struct TypeDescriptor_StdSharedPtr : TypeDescriptor
		{
			TypeDescriptor* targetType;
			const void* (*getTarget)(const void*);
			void (*alloc)(void*);

			//template constructor
			template<typename TargetType>
			TypeDescriptor_StdSharedPtr(TargetType*):TypeDescriptor("std::shared_ptr<>",
			sizeof(std::shared_ptr<TargetType>)), targetType(TypeResolver<TargetType>::get())
			{
				getTarget = [](const void* SharedPtrPtr)->const void*
				{
					const auto& SharedPtr = *(const std::shared_ptr<TargetType>*)SharedPtrPtr;
					return SharedPtr.get();
				};

				alloc = [](void* SharedPtrPtr)->void
				{
					auto& SharedPtr = *(std::shared_ptr<TargetType>*)SharedPtrPtr;
					SharedPtr = CreateRef<TargetType>();
				};
			}
			virtual std::string getFullName() const override
			{
				return std::string("std::shared_ptr<") + targetType->getFullName() + ">";
			}
			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name, bool isArrayItem) override
			{
				targetType->Write(writer, const_cast<void*>(getTarget(obj)), name, isArrayItem);
			}

			virtual void Read(rapidjson::Value& doc, void* obj, const char* name, bool isArrayItem) override
			{
				alloc(obj);
				targetType->Read(doc, const_cast<void*>(getTarget(obj)), name, isArrayItem);
			}

		};
		template<typename T>
		struct TypeResolver<std::shared_ptr<T>>
		{
			static TypeDescriptor* get()
			{
				static TypeDescriptor_StdSharedPtr typeDesc(static_cast<T*>(nullptr));
				return &typeDesc;
			}
		};
		//------std::shared_ptr<>------

		//------std::map<>------
		struct TypeDescriptor_StdMap : TypeDescriptor
		{
			TypeDescriptor* keyType;
			TypeDescriptor* valueType;

			void* (*getBegin)(const void*);
			void* (*getEnd)(const void*);
			bool (*NotEqual)(void*, void*);
			void* (*getNext)(void*);
			const void* (*getKey)(void*);
			void* (*getValue)(void*);
			
			template<typename KeyType, typename ValueType>
			TypeDescriptor_StdMap(KeyType*, ValueType*) :TypeDescriptor("std::map<>", sizeof(std::map<KeyType, ValueType>)),
				keyType(TypeResolver<KeyType>::get()), valueType(TypeResolver<ValueType>::get())
			{
				getBegin = [](const void* mapPtr)->void* {
					const auto& mp = *(const std::map<KeyType, ValueType>*)mapPtr;

					//get the iterator
					return &mp.begin();
				};

				getEnd = [](const void* mapPtr)->void* {
					const auto& mp = *(const std::map<KeyType, ValueType>*)mapPtr;

					//get the iterator
					return &mp.end();
				};

				NotEqual = [](void* iter1, void* iter2)->bool {
					auto& mpIter1 = *(std::map<KeyType, ValueType>::iterator*)iter1;
					auto& mpIter2 = *(std::map<KeyType, ValueType>::iterator*)iter2;
					return mpIter1 != mpIter2;
				};

				getNext = [](void* iter)->void* {
					auto& mpIter = *(std::map<KeyType, ValueType>::iterator*)iter;
					return &(++mpIter);
				};

				getKey = [](void* iter)->const void* {
					auto& mpIter = *(std::map<KeyType, ValueType>::iterator*)iter;
					return &(mpIter->first);
				};

				getValue = [](void* iter)->void* {
					auto& mpIter = *(std::map<KeyType, ValueType>::iterator*)iter;
					return &(mpIter->second);
				};
			}

			virtual std::string getFullName() const override
			{
				//return std::string("std::map<") + itemType->getFullName() + ">";
				return std::string("std::map<") + keyType->getFullName() + ", " + valueType->getFullName() + ">";
			}

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name, bool isArrayItem) override
			{
				if(!isArrayItem)
					writer.Key(name);
				writer.StartArray();
				
				void* begin = getBegin(obj);
				void* end = getEnd(obj);
				while (NotEqual(begin, end))
				{
					writer.StartObject();
					keyType->Write(writer, const_cast<void*>(getKey(begin)), keyType->name, false);
					valueType->Write(writer, getValue(begin), valueType->name, false);			
					writer.EndObject();
					getNext(begin);
				}
				//for (size_t i = 0; i < vecSize; ++i)
				//{
				//	itemType->Write(writer, const_cast<void*>(getItem(obj, i)), itemType->name);
				//}
				
				writer.EndArray();
			}

			virtual void Read(rapidjson::Value& doc, void* obj, const char* name, bool isArrayItem) override
			{
				//
			}

		};

		//partially specialize TypeResolver<> for std::maps:
		template<typename Key, typename Value>
		struct TypeResolver<std::map<Key, Value>>
		{
			static TypeDescriptor* get()
			{
				static TypeDescriptor_StdMap typeDesc(static_cast<Key*>(nullptr), static_cast<Value*>(nullptr));
				return &typeDesc;
			}
		};
		//------std::vector<>------

#define REFLECT() \
    friend struct Reflect::DefaultResolver; \
    static Reflect::TypeDescriptor_Struct Reflection; \
    static void initReflection(Reflect::TypeDescriptor_Struct*);

#define REFLECT_STRUCT_BEGIN(type) \
    Reflect::TypeDescriptor_Struct type::Reflection{type::initReflection}; \
    void type::initReflection(Reflect::TypeDescriptor_Struct* typeDesc) { \
        using T = type; \
        typeDesc->name = #type; \
        typeDesc->size = sizeof(T); \
        typeDesc->members = {

#define REFLECT_STRUCT_MEMBER(name) \
            {#name, offsetof(T, name), Reflect::TypeResolver<decltype(T::name)>::get()},

#define REFLECT_STRUCT_END() \
        }; \
    }
		}
}
