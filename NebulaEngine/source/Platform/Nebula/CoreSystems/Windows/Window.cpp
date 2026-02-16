#include "Window.h"

#ifdef NB_PLATFORM_WINDOWS
#include "Platform/Platform/Windows/Windows/WindowsWindow.h"
#elif NB_PLATFORM_LINUX
#include "Platform/Platform/Linux/Windows/LinuxWindow.h"
#endif

namespace Nebula::Platform
{

    Ref<Window> Window::Create(const WindowProperties& props)
    {

#ifdef NB_PLATFORM_WINDOWS
        return CreateRef<WindowsWindow>(props);
#elif NB_PLATFORM_LINUX
        return CreateRef<LinuxWindow>(props);
#endif

    }

}