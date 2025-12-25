#include "Window.h"

#include "Platform/Platform/Windows/Windows/WindowsWindow.h"

namespace Nebula::Platform
{

    Ref<Window> Window::Create(const WindowProperties& props)
    {

#ifdef NB_PLATFORM_WINDOWS
        return CreateRef<WindowsWindow>(props);
#endif

    }

}