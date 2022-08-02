#include "pxpch.h"
#include "Reflect.h"

namespace Pixel {
	namespace reflect {
		
		//------primitive type descriptor------
		struct TypeDescriptor_Int : TypeDescriptor {
			TypeDescriptor_Int() : TypeDescriptor{ "int", sizeof(int) } {

			}
		};

		template<>
		TypeDescriptor* getPrimitiveDescriptor<int>() {
			static TypeDescriptor_Int typeDesc;
			return &typeDesc;
		}
		//------primitive type descriptor------
	}	
}