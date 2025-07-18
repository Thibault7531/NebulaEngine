#include "Window.h"

#include "Platform/Windows/WindowsWindow.h"

namespace Nebula {

    std::vector<Ref<Window>> Window::m_Windows = {};

    Ref<Window> Window::GetWindow(uint32_t windowId)
    {
        for (int i = m_Windows.size() - 1; i >= 0; --i) {
            if (m_Windows[i]->GetID() == windowId) return m_Windows[i];
        }

        return nullptr;
    }

    void Window::DestroyWindow(uint32_t windowID)
    {
        for (auto it = m_Windows.begin(); it != m_Windows.end(); ++it) {
            if ((*it)->GetID() == windowID) {
                (*it)->Shutdown();
                m_Windows.erase(it);
                return;
            };
        }
    }

    Ref<Window> Window::Create(const WindowProps& props)
    {
        const Ref<WindowsWindow> window = CreateRef<WindowsWindow>(props);
        m_Windows.push_back(window);
        return window;
    }

}
