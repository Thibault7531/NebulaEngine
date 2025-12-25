#pragma once

#include "Core/Nebula/CoreSystems/Core/Core.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"

#undef CreateWindow

namespace Nebula::Platform
{

    class WindowManager
    {
    public:
        WindowManager() = default;
        ~WindowManager() = default;

        Ref<Window> CreateMainWindow(const WindowProperties& props);
        Ref<Window> GetMainWindow();
        void DestroyMainWindow();

        Ref<Window> CreateWindow(const WindowProperties& props);
        Ref<Window> GetWindowFromID(int WindowID);
        Ref<Window> GetWindowFromHandle(void* handle);
        void DestroyWindow(Ref<Window> window);

        static WindowManager& Get() { return *s_Instance; }

    private:
        Ref<Window> m_MainWindow;
        std::vector<Ref<Window>> m_Windows;

        static WindowManager* s_Instance;
    };

}
