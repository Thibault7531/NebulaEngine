#pragma once

#include <SDL_video.h>

#include "Nebula/Renderer/Core/GraphicsContext.h"

namespace Nebula {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(SDL_Window* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual void SetCurrent() override;

	private:
		SDL_Window* m_WindowHandle;
		SDL_GLContext m_Context;
	};

}
