#include "OpenGL460Surface.h"

#include <glad/gl.h>
#include <KHR/khrplatform.h>

#include "Platform/Nebula/CoreSystems/Core/Log.h"
#include "Platform/Platform/Linux/Windows/LinuxWindow.h"

namespace Nebula::Graphics
{
    bool OpenGL460Surface::s_GLInitialized = false;
    SDL_GLContext OpenGL460Surface::s_MainContext = nullptr;

    OpenGL460Surface::OpenGL460Surface(Platform::Window* window)
        : m_Window(window)
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
        if (!m_Context)
        {
            NB_PLATFORM_ERROR("Failed to create OpenGL context: {0}", SDL_GetError());
            return;
        }

        if (!s_GLInitialized)
        {
            if (!gladLoadGL(SDL_GL_GetProcAddress)) NB_PLATFORM_ERROR("Failed to load OpenGL functions");
            s_GLInitialized = true;
            s_MainContext = m_Context;
        }
    }

    OpenGL460Surface::~OpenGL460Surface()
    {
        SDL_GL_DestroyContext(m_Context);
    }

    std::string OpenGL460Surface::GetAPIVersion() const
    {
        return reinterpret_cast<const char*>(glGetString(GL_VERSION));
    }

    std::string OpenGL460Surface::GetAPIVendor() const
    {
        return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    }

    std::string OpenGL460Surface::GetAPIRenderer() const
    {
        return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    }
}
