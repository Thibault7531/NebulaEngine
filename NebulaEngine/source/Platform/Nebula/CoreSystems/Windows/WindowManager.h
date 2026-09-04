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

        std::vector<Ref<Window>>::iterator begin() { return m_Windows.begin(); }
        std::vector<Ref<Window>>::iterator end() { return m_Windows.end(); }
        std::vector<Ref<Window>>::reverse_iterator rbegin() { return m_Windows.rbegin(); }
        std::vector<Ref<Window>>::reverse_iterator rend() { return m_Windows.rend(); }

        std::vector<Ref<Window>>::const_iterator cbegin() { return m_Windows.begin(); }
        std::vector<Ref<Window>>::const_iterator cend() { return m_Windows.end(); }
        std::vector<Ref<Window>>::const_reverse_iterator crbegin() { return m_Windows.rbegin(); }
        std::vector<Ref<Window>>::const_reverse_iterator crend() { return m_Windows.rend(); }

    private:
        Ref<Window> m_MainWindow;
        std::vector<Ref<Window>> m_Windows;

        static WindowManager* s_Instance;
    };

}
