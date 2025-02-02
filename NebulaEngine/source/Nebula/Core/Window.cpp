#include "Window.h"

#include "Platform/Windows/WindowsWindow.h"

namespace Nebula {

    std::vector<Ref<Window>> Window::m_Windows = {};

    Ref<Window> Window::GetWindow(uint32_t windowId)
    {
        for (auto window : m_Windows) {
            if (window->GetID() == windowId) return window;
        }

        return nullptr;
    }

    void Window::DestroyWindow(Ref<Window> window)
    {
        auto it = std::ranges::find(m_Windows, window);
        if (it == m_Windows.end()) m_Windows.erase(it);
    }

    Ref<Window> Window::Create(const WindowProps& props)
    {
        const Ref<WindowsWindow> window = CreateRef<WindowsWindow>(props);
        m_Windows.push_back(window);
        return window;
    }

}
