#pragma once

namespace Nebula {
	/**
	 * Class to handle time steps.
	 */
	class Timestep
	{
	public:
		Timestep(float time = 0.0f)
			: m_Time(time)
		{
		}

		/**
		 * Gets the time step in seconds by default.
		 */
		operator float() const { return m_Time; }

		/**
		 * Gets the time step in seconds.
		 * @return Time step in seconds
		 */
		float GetSeconds() const { return m_Time; }
		/**
		 * Gets the time step in milliseconds.
		 * @return Time step in milliseconds
		 */
		float GetMilliseconds() const { return m_Time * 1000.0f; }

	private:
		float m_Time;
	};

}
