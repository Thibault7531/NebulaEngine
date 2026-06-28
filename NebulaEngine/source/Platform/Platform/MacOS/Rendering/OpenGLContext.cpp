#include "OpenGLContext.h"

#include <glad/gl.h>

#include "WindowManager.h"
#include "Platform/Nebula/CoreSystems/Core/Log.h"

namespace Nebula::Platform
{
    OpenGlContext::OpenGlContext(int32_t windowID)
        : m_NativeContext{SwiftOpenGLContext::CreateOpenGLContext(windowID)}, m_WindowID{windowID}
    {
        int result = gladLoaderLoadGL();
        if (!result)
        {
            NB_PLATFORM_ERROR("Failed to initialize Glad!");
        }
    }

    void OpenGlContext::Init() {}

    void OpenGlContext::SetCurrent()
    {
        m_NativeContext.SetCurrent();
    }

    void OpenGlContext::SetVSync(VSyncMode mode)
    {
        m_NativeContext.SetVSync(mode);
    }

    VSyncMode OpenGlContext::IsVsync() const
    {
        return m_NativeContext.IsVSync();
    }

    void OpenGlContext::SwapBuffers()
    {
        m_NativeContext.SwapBuffers();
    }

    void OpenGlContext::Present(uint32_t framebuffer)
    {
        Ref<Window> window = WindowManager::Get().GetWindowFromID(m_WindowID);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, window->GetSize().first, window->GetSize().second, 0, 0, window->GetSize().first, window->GetSize().second, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        SwapBuffers();
    }
}
