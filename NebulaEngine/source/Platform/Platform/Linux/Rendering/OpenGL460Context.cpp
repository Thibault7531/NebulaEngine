#include "OpenGL460Context.h"

#include <glad/gl.h>
#include <KHR/khrplatform.h>

#include "Platform/Nebula/CoreSystems/Core/Log.h"
#include "Platform/Platform/Linux/Windows/LinuxWindow.h"

namespace Nebula::Platform
{
    bool OpenGL460Context::s_GLInitialized = false;
    SDL_GLContext OpenGL460Context::s_MainContext = nullptr;

    OpenGL460Context::OpenGL460Context(Window* window)
        : m_Window(window)
    {
    }

    OpenGL460Context::~OpenGL460Context()
    {
        SDL_GL_DestroyContext(m_Context);
    }

    void OpenGL460Context::Init()
    {
        if (!s_GLInitialized)
        {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        }
        else
        {
            SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
            SDL_GL_MakeCurrent(static_cast<SDL_Window*>(m_Window->GetNativeWindow()), s_MainContext);
        }

        m_Context = SDL_GL_CreateContext(static_cast<SDL_Window*>(m_Window->GetNativeWindow()));
        SetCurrent();

        if (!s_GLInitialized)
        {
            if (!gladLoadGL(SDL_GL_GetProcAddress)) NB_PLATFORM_ERROR("Failed to load OpenGL functions");
            s_GLInitialized = true;
            s_MainContext = m_Context;
        }
    }

    void OpenGL460Context::SetCurrent()
    {
        SDL_GL_MakeCurrent(static_cast<SDL_Window*>(m_Window->GetNativeWindow()), m_Context);
    }

    void OpenGL460Context::SetVSync(VSyncMode mode)
    {
        if (mode == VSyncMode::Off) SDL_GL_SetSwapInterval(0);
        if (mode == VSyncMode::Adaptive) SDL_GL_SetSwapInterval(-1);
        if (mode == VSyncMode::On) SDL_GL_SetSwapInterval(1);
    }

    VSyncMode OpenGL460Context::IsVsync() const
    {
        int mode;
        SDL_GL_GetSwapInterval(&mode);

        if (mode == -1) return VSyncMode::Adaptive;
        if (mode == 1) return VSyncMode::On;
        return VSyncMode::Off;
    }

    void OpenGL460Context::SwapBuffers()
    {
        SDL_GL_SwapWindow(static_cast<SDL_Window*>(m_Window->GetNativeWindow()));
    }

    void OpenGL460Context::Present(uint32_t framebuffer)
    {
        glBlitNamedFramebuffer(framebuffer, 0, 0, 0, m_Window->GetSize().first, m_Window->GetSize().second, 0, 0, m_Window->GetSize().first, m_Window->GetSize().second, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        SwapBuffers();
    }
}
