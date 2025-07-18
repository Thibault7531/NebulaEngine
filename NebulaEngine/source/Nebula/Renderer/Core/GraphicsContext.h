#pragma once

namespace Nebula {

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		virtual void SetCurrent() = 0;
	};

}