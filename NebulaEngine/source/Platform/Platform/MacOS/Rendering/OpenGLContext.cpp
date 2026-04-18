#include "OpenGLContext.h"

#include <glad/gl.h>

#include "Platform/Nebula/CoreSystems/Core/Log.h"

namespace Nebula::Platform
{
    OpenGlContext::OpenGlContext(int32_t windowID)
        : m_NativeContext{SwiftOpenGLContext::CreateOpenGLContext(windowID)}
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
}
