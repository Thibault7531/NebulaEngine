#include "WindowManager.h"

namespace Nebula::Platform
{

    WindowManager* WindowManager::s_Instance = new WindowManager;

    Ref<Window> WindowManager::CreateMainWindow(const WindowProperties& props)
    {
        m_MainWindow = Window::Create(props);
        m_Windows.push_back(m_MainWindow);
        return m_MainWindow;
    }

    Ref<Window> WindowManager::GetMainWindow()
    {
        return m_MainWindow;
    }

    void WindowManager::DestroyMainWindow()
    {
        bool found = false;
        int32_t i = 0;
        while (!found && i < m_Windows.size())
        {
            if (m_Windows[i] == m_MainWindow)
            {
                m_Windows.erase(m_Windows.begin() + i);
                found = true;
            }
            i++;
        }

        m_MainWindow.reset();
    }

    Ref<Window> WindowManager::CreateWindow(const WindowProperties& props)
    {
        Ref<Window> window = Window::Create(props);
        m_Windows.push_back(window);
        return window;
    }

    Ref<Window> WindowManager::GetWindowFromID(int WindowID)
    {
        int32_t i = 0;
        while (i < m_Windows.size())
        {
            if (m_Windows[i]->GetID() == WindowID) return m_Windows[i];
            i++;
        }
        return nullptr;
    }

    Ref<Window> WindowManager::GetWindowFromHandle(void* handle)
    {
        int32_t i = 0;
        while (i < m_Windows.size())
        {
            if (m_Windows[i]->GetHandle() == handle) return m_Windows[i];
            i++;
        }
        return nullptr;
    }

    void WindowManager::DestroyWindow(Ref<Window> window)
    {
        int32_t i = 0;
        while (i < m_Windows.size())
        {
            if (m_Windows[i] == window)
            {
                m_Windows.erase(m_Windows.begin() + i);
                return;
            }
            i++;
        }
    }
}
