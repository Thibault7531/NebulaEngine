#include "WindowManager.h"

namespace Nebula
{
    WindowManager* WindowManager::s_Instance = new WindowManager();

    Ref<Window> WindowManager::CreateNewMainWindow(const WindowProps& props)
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
        auto it = std::ranges::find(m_Windows, m_MainWindow);
        if (it != m_Windows.end())
        {
            (*it)->Shutdown();
            m_Windows.erase(it);
        }
        m_MainWindow.reset();
    }

    Ref<Window> WindowManager::CreateNewWindow(const WindowProps& props)
    {
        auto window = Window::Create(props);
        m_Windows.push_back(window);
        return window;
    }

    Ref<Window> WindowManager::GetWindow(int WindowID)
    {
        for (int i =0; i < m_Windows.size(); i++) {
            if (m_Windows[i]->GetID() == WindowID) return m_Windows[i];
        }

        return nullptr;
    }

    void WindowManager::DestroyWindow(Ref<Window> window)
    {
        auto it = std::ranges::find(m_Windows, window);
        if (it != m_Windows.end())
        {
            (*it)->Shutdown();
            m_Windows.erase(it);
        }
    }
}
