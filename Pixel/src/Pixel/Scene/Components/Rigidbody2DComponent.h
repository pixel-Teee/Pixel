#pragma once

namespace Pixel {
	//Physics

	struct Rigidbody2DComponent
	{
		enum class BodyType {
			Static = 0,
			Dynamic,
			Kinematic
		};

		BodyType Type = BodyType::Static;

		bool FixedRotation = false;

		//Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};
}