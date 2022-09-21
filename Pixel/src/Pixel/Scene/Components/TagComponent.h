#pragma once

namespace Pixel {
	struct Meta(Enable) TagComponent
	{
		Meta()
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			:Tag(tag) {}


		RTTR_REGISTRATION_FRIEND
		RTTR_ENABLE()
	};
}