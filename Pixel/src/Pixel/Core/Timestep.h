#pragma once

namespace Pixel {

	class Timestep
	{
	public:
		Timestep(float time = 0.0f)
		:m_Time(time)
		{

		}

		operator float(){return m_Time;}

		float GetSeconds() const { return m_Time; }
		float GetMilliseconds() const { return m_Time * 1000.0f; }
	private:
		float m_Time = 0.0f;
	};
}