#pragma once

#include <string>
#include <vector>

namespace Pixel {
	namespace reflect {
		//------
		//base class of all type descriptors
		//------

		struct TypeDescriptor {
			const char* name;//type name
			size_t size;

			TypeDescriptor(const char* name, size_t size) : name{ name }, size{ size }{}
			virtual ~TypeDescriptor() {}
			virtual std::string GetFullName() const { return name; }

			//dump?
		};

		//------
		//finding type descriptors
		//------

		//declare the function template that handles primitive types such as int, std::string, etc
		template<typename T>
		TypeDescriptor* getPrimitiveDescriptor();

		//a helper class to find TypeDescriptors in different ways:
		struct DefaultResolver {
			template<typename T> static char func(decltype(&T::Reflection));
			template<typename T> static int func(...);//sfinae

			template<typename T>
			struct IsReflected {
				static constexpr bool value = sizeof(func<T>(nullptr)) == sizeof(char);
			};

			//this version is called if T has a static member named "Reflection"
			template<typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
			struct TypeDescriptor* get() {
				return &T::Reflection;
			}

			//this version is called otherwise:
			template<typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
			static TypeDescriptor* get() {
				return getPrimitiveDescriptor<T>();
			}
		};

		//this is the primary class template for finding all TypeDescriptors:
		template<typename T>
		struct TypeResolver {
			static TypeDescriptor* get() {
				return DefaultResolver::get<T>();
			}
		};

		//------
		//type descriptors for user-defined structs/classes
		//------
		struct TypeDescriptor_Struct : TypeDescriptor {
			struct Member {
				const char* name;
				size_t offset;
				TypeDescriptor* type;
			};

			std::vector<Member> members;

			TypeDescriptor_Struct(void (*init)(TypeDescriptor_Struct*)) : TypeDescriptor(nullptr, 0) {
				init(this);
			}

			TypeDescriptor_Struct(const char* name, size_t size, const std::initializer_list<Member>& init) :
				TypeDescriptor{ nullptr, 0 }, members{ init }
			{

			}
		};

#define REFLECT()\
	friend struct reflect::DefaultResolver; \
	static reflect::TypeDescriptor_Struct Reflection; \
	static void initReflection(reflect::TypeDescriptor_Struct*);

#define REFLECT_STRUCT_BEGIN(type)\
	reflect::TypeDescriptor_Struct type::Reflection{type::initReflection};\
	void type::initReflection(reflect::TypeDescriptor_Struct* typeDesc){\
		using T = type;\
		typeDesc->name = #type;\
		typeDesc->size = sizeof(T);\
		typeDesc->members = {

#define REFLECT_STRUCT_MEMBER(name)\
	{#name, offsetof(T, name), reflect::TypeResolver<decltype(T::name)>::get()},

#define REFLECT_STRUCT_END()\
		};\
	}
	}	
}