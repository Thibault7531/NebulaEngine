#pragma once

#include <EGL/egl.h>
#include <wayland-egl.h>

#include "Platform/Nebula/CoreSystems/Rendering/Context.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"

namespace Nebula::Platform
{

    class OpenGlContext : public Context
    {
    public:
        OpenGlContext(Window* window);
        virtual ~OpenGlContext();

        virtual void Init() override;

        virtual void SetCurrent() override;
        virtual void SetVSync(VSyncMode mode) override;
        virtual VSyncMode IsVsync() override;
        virtual void SwapBuffers() override;

    private:
        wl_egl_window* m_NativeWindow;
        EGLDisplay m_Display = nullptr;
        EGLConfig m_Config = nullptr;
        EGLSurface m_Surface = nullptr;
        EGLContext m_Context = nullptr;

    private:
        static bool s_GLInitialized;
        static EGLContext s_MainContext;
    };

}
