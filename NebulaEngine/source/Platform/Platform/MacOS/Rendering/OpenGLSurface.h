#pragma once

#include "Window.h"
#include "Platform/Nebula/CoreSystems/Graphics/Surface.h"

#include <SwiftOpenGLSurface.h>

namespace Nebula::Graphics
{

    class OpenGlSurface : public Surface
    {
    public:
        OpenGlSurface(int32_t windowID);
        virtual ~OpenGlSurface() = default;

        virtual void SetDevice(void* device, void* commandQueue) override {}

        virtual void SetVSync(VSyncMode mode) override;
        virtual VSyncMode IsVsync() const override;

        virtual void NewFrame() override;
        virtual void Present(void* framebuffer) override;
        
    private:
        SwiftOpenGLSurface::OpenGLSurface m_NativeSurface;

        int32_t m_WindowID;
    };

}
