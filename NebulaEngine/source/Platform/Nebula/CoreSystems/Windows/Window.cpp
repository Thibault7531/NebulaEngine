#include "Window.h"

#ifdef NB_PLATFORM_WINDOWS
#include "Platform/Platform/Windows/Windows/WindowsWindow.h"
#elif NB_PLATFORM_LINUX
#include "Platform/Platform/Linux/Windows/LinuxWindow.h"
#elif NB_PLATFORM_MACOS
#include "Platform/Platform/MacOS/Windows/MacOSWindow.h"
#endif

namespace Nebula::Platform
{

    Ref<Window> Window::Create(const WindowProperties& props)
    {

#ifdef NB_PLATFORM_WINDOWS
        return CreateRef<WindowsWindow>(props);
#elif NB_PLATFORM_LINUX
        return CreateRef<LinuxWindow>(props);
#elif NB_PLATFORM_MACOS
        return CreateRef<MacOSWindow>(props);
#endif

    }

    std::string Window::GetWindowStyleName(WindowStyle style)
    {
        switch (style)
        {
        case WindowStyle::Default: return "Default";
        case WindowStyle::BorderlessInteractive: return "Borderless Interactive";
        case WindowStyle::BorderlessFullscreen: return "Borderless Fullscreen";
        case WindowStyle::ExclusiveFullscreen: return "Exclusive Fullscreen";
        }
    }
}
