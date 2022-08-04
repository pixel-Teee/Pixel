#pragma once

#include <yaml-cpp/yaml.h>

#include "Pixel/Core/Stream.h"

namespace Pixel {
	class YAMLStream : public Stream
	{
	public:
		YAMLStream();

		virtual ~YAMLStream() override;

	private:
		YAML::Emitter out;
	};
}