#pragma once

#include "Window.h"
#include "Platform/Nebula/CoreSystems/Rendering/Context.h"

#include <SwiftOpenGLContext.h>

namespace Nebula::Platform
{

    class OpenGlContext : public Context
    {
    public:
        OpenGlContext(int32_t windowID);
        virtual ~OpenGlContext() = default;

        virtual void Init() override;

        virtual void SetCurrent() override;
        virtual void SetVSync(VSyncMode mode) override;
        virtual VSyncMode IsVsync() const override;
        virtual void SwapBuffers() override;
        virtual void Present(uint32_t framebuffer) override;
        
    private:
        SwiftOpenGLContext::OpenGLContext m_NativeContext;

        int32_t m_WindowID;
    };

}
