#pragma once

#include <SDL_video.h>

#include "Nebula/Renderer/GraphicsContext.h"

namespace Nebula {
	/**
	 * OpenGL Context.
	 */
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(SDL_Window* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

		SDL_GLContext GetContext() { return m_Context; }

	private:
		SDL_Window* m_WindowHandle;
		SDL_GLContext m_Context;
	};

}
