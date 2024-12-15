#include "OpenGLContext.h"

#include <glad/glad.h>

namespace Nebula {

	OpenGLContext::OpenGLContext(SDL_Window* windowHandle)
		: m_WindowHandle(windowHandle), m_Context(nullptr)
	{
	}

	void OpenGLContext::Init()
	{
		m_Context = SDL_GL_CreateContext(m_WindowHandle);

		int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress));
		NB_CORE_ASSERT(status, "Failed to initialize Glad!");

		NB_CORE_INFO("OpenGL Info:");
		NB_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		NB_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		NB_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		SDL_GL_SwapWindow(m_WindowHandle);
	}

}
