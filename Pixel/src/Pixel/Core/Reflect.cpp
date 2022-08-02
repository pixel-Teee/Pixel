#include "pxpch.h"
#include "Reflect.h"

namespace Pixel {
	namespace reflect {
		struct TypeDescriptor_Int : TypeDescriptor {
			TypeDescriptor_Int() : TypeDescriptor{ "int", sizeof(int) } {

			}
		};

		template<>
		TypeDescriptor* getPrimitiveDescriptor<int>() {
			static TypeDescriptor_Int typeDesc;
			return &typeDesc;
		}
	}	
}