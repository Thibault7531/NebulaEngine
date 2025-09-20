#include "Window.h"

#include "Platform/Windows/WindowsWindow.h"

namespace Nebula {

    Ref<Window> Window::Create(const WindowProps& props)
    {
        return CreateRef<WindowsWindow>(props);
    }

}
