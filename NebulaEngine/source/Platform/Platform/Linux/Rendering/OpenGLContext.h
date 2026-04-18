#pragma once

#include <EGL/egl.h>
#include <SDL3/SDL.h>

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
        virtual VSyncMode IsVsync() const override;
        virtual void SwapBuffers() override;

    private:
        Window* m_Window;
        SDL_GLContext m_Context = nullptr;

    private:
        static bool s_GLInitialized;
        static SDL_GLContext s_MainContext;
    };

}
