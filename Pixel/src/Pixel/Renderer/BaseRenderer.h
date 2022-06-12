#pragma once

namespace Pixel {
	class BufferLayout;
	class Device;
	//renderer interface
	class BaseRenderer
	{
	public:
		virtual ~BaseRenderer();

		virtual void CreatePso(BufferLayout& layout) = 0;

		static Ref<BaseRenderer> Create();
	};
}