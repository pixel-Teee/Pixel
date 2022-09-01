#include "pxpch.h"

#include "MaterialCustomPara.h"

namespace Pixel {

	MaterialCustomPara::~MaterialCustomPara()
	{

	}

}


RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<Pixel::MaterialCustomPara>("MaterialCustomPara")
		.constructor<>()
		.property("ConstValueName", &Pixel::MaterialCustomPara::ConstValueName);
}