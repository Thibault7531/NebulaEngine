#include "OpenGLSurface.h"

#include <glad/gl.h>

#include "WindowManager.h"
#include "Platform/Nebula/CoreSystems/Core/Log.h"

namespace Nebula::Graphics
{
    OpenGlSurface::OpenGlSurface(int32_t windowID)
        : m_NativeSurface{SwiftOpenGLSurface::CreateOpenGLSurface(windowID)}, m_WindowID{windowID}
    {
        int result = gladLoaderLoadGL();
        if (!result)
        {
            NB_PLATFORM_ERROR("Failed to initialize Glad!");
        }
    }

    void OpenGlSurface::SetVSync(VSyncMode mode)
    {
        m_NativeSurface.SetVSync(mode);
    }

    VSyncMode OpenGlSurface::IsVsync() const
    {
        return m_NativeSurface.IsVSync();
    }

    void OpenGlSurface::Resize(uint32_t width, uint32_t height)
    {
        NewFrame();
        glViewport(0, 0, width, height);
    }

    void OpenGlSurface::NewFrame()
    {
        m_NativeSurface.NewFrame();
    }

    void OpenGlSurface::Present(void* framebuffer)
    {
        Ref<Platform::Window> window = Platform::WindowManager::Get().GetWindowFromID(m_WindowID);

        if (reinterpret_cast<uint64_t>(framebuffer) != 0)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, reinterpret_cast<uint64_t>(framebuffer));
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, window->GetSize().first, window->GetSize().second, 0, 0, window->GetSize().first, window->GetSize().second, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }

        m_NativeSurface.SwapBuffers();
    }
}
