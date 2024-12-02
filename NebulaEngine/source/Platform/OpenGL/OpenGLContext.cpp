#include "nbpch.h"
#include "OpenGLContext.h"

#include "glad/glad.h"

namespace Nebula {

	OpenGLContext::OpenGLContext(SDL_Window* windowHandle)
		: m_WindowHandle(windowHandle), m_Context(nullptr)
	{
	}

	void OpenGLContext::Init()
	{
		m_Context = SDL_GL_CreateContext(m_WindowHandle);

		int status = gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
		NB_CORE_ASSERT(status, "Failed to initialize Glad!");
	}

	void OpenGLContext::SwapBuffers()
	{
		SDL_GL_SwapWindow(m_WindowHandle);
		glClear(GL_COLOR_BUFFER_BIT);
	}

}
