#pragma once

#include <string>
#include <vector>

namespace rapidjson
{
	template<typename OutputStream, typename SourceEncoding = UTF8<>, typename TargetEncoding = UTF8<>, typename StackAllocator = CrtAllocator, unsigned writeFlags = kWriteDefaultFlags>
	class Writer;
	class StringBuffer;
	class Document;
}

namespace Pixel {
	namespace Reflect
	{
		struct TypeDescriptor {
			const char* name;
			size_t size;

			TypeDescriptor(const char* name, size_t size) : name{ name }, size{ size } {}
			virtual ~TypeDescriptor() {}
			virtual std::string getFullName() const { return name; }

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name) = 0;//write to json
			virtual void Read(rapidjson::Document& doc, void* obj, const char* name) = 0;//read to obj
			virtual void Read(rapidjson::Value& value, void* obj, const char* name) = 0;
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

			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name) override;
			virtual void Read(rapidjson::Document& doc, void* obj, const char* name) override;

			// Inherited via TypeDescriptor
			virtual void Write(rapidjson::Writer<rapidjson::StringBuffer>& writer, void* obj, const char* name) override;
			virtual void Read(rapidjson::Document& doc, void* obj, const char* name) override;
			virtual void Read(rapidjson::Value& value, void* obj, const char* name) override;
		};

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
