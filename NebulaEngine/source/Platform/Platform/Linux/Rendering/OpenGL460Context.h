#pragma once

#include <EGL/egl.h>
#include <SDL3/SDL.h>

#include "Platform/Nebula/CoreSystems/Rendering/Context.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"

namespace Nebula::Platform
{

    class OpenGL460Context : public Context
    {
    public:
        OpenGL460Context(Window* window);
        virtual ~OpenGL460Context();

        virtual void Init() override;

        virtual void SetCurrent() override;
        virtual void SetVSync(VSyncMode mode) override;
        virtual VSyncMode IsVsync() const override;
        virtual void SwapBuffers() override;
        virtual void Present(uint32_t framebuffer) override;

    private:
        Window* m_Window;
        SDL_GLContext m_Context = nullptr;

    private:
        static bool s_GLInitialized;
        static SDL_GLContext s_MainContext;
    };

}
