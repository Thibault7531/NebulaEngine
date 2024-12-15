#pragma once

namespace Nebula {
	/**
	 * Class representing a graphics context.
	 */
	class GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};

}