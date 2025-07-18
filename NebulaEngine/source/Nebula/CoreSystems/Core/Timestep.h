#pragma once

namespace Nebula {

	class Timestep
	{
	public:
		Timestep(float time = 0.0f)
			: m_Time(time)
		{
		}

		operator double() const { return m_Time; }

		double GetSeconds() const { return m_Time; }
		double GetMilliseconds() const { return m_Time * 1000.0f; }
		double GetMicroseconds() const { return m_Time * 1000000.0f; }
		double GetNanoseconds() const { return m_Time * 1000000000.0f; }

	private:
		double m_Time;
	};

}
