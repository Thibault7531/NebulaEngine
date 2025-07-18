#include "OpenGLContext.h"

#include <glad/glad.h>

#include "Nebula/CoreSystems/Core/Core.h"

namespace Nebula {

	OpenGLContext::OpenGLContext(SDL_Window* windowHandle)
		: m_WindowHandle(windowHandle), m_Context(nullptr)
	{
	}

	void OpenGLContext::Init()
	{
		NB_PROFILE_FUNCTION();

		m_Context = SDL_GL_CreateContext(m_WindowHandle);

		int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress));
		NB_CORE_ASSERT(status, "Failed to initialize Glad!");

	#ifdef NB_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		NB_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 6), "Nebula requires OpenGL version 4.6")
	#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		NB_PROFILE_FUNCTION();

		SDL_GL_SwapWindow(m_WindowHandle);
	}

	void OpenGLContext::SetCurrent()
	{
		SDL_GL_MakeCurrent(m_WindowHandle, m_Context);
	}
}
