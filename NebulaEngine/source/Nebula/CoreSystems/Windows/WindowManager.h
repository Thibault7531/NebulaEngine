#pragma once

#include "Window.h"
#include "Nebula/CoreSystems/Core/Core.h"

namespace Nebula
{

    class WindowManager
    {
    public:
        WindowManager() = default;
        ~WindowManager() = default;

        Ref<Window> CreateNewMainWindow(const WindowProps& props);
        Ref<Window> GetMainWindow();
        void DestroyMainWindow();

        Ref<Window> CreateNewWindow(const WindowProps& props);
        Ref<Window> GetWindow(int WindowID);
        Ref<Window> GetWindow(void* platformHandle);
        void DestroyWindow(Ref<Window> window);

        static WindowManager& Get() { return *s_Instance; }

    private:
        Ref<Window> m_MainWindow;
        std::vector<Ref<Window>> m_Windows;

        static WindowManager* s_Instance;
    };

}
