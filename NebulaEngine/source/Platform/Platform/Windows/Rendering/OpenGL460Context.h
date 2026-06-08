#pragma once

#include <Windows.h>
#include <winrt/Microsoft.UI.Windowing.h>

#include "Core/Nebula/CoreSystems/Core/Core.h"
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

        void InitOpenGL();

    private:
        Window* m_Window;
        HWND m_Handle;
        HDC m_DeviceContext;

        HGLRC m_Context = nullptr;

    private:
        static bool s_GLInitialized;
        static HGLRC s_MainContext;
    };

}
