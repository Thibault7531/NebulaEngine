#include "Platform/Nebula/CoreSystems/Core/PlatformUtils.h"

#include <mach-o/dyld.h>

#include "Log.h"
#include "Platform/Nebula/CoreSystems/Input/KeyCodes.h"
#include "SwiftPlatformUtils.h"
#include "WindowManager.h"

namespace Nebula::Platform
{
    std::string Files::s_AppName = "";

    struct MacOSCursorWrapper
    {
        SwiftPlatformUtils::MacOSCursor m_Cursor;

        MacOSCursorWrapper(CursorType type)
            : m_Cursor{SwiftPlatformUtils::createMacOSCursor(type)}
        {
        }
    };

    std::vector<Display> Displays::GetDisplays()
    {
        uint32_t number = SwiftPlatformUtils::GetDisplayAmount();
        std::vector<Display> result;
        result.reserve(number);

        for (uint32_t i = 0; i < number; i++)
        {
            result.push_back(SwiftPlatformUtils::GetDisplay(i));
        }

        return result;
    }

    void Mouse::ShowCursor()
    {
        SwiftPlatformUtils::ShowCursor();
    }

    void Mouse::HideCursor()
    {
        SwiftPlatformUtils::HideCursor();
    }

    void Mouse::SetMousePosGlobal(int32_t x, int32_t y)
    {
        SwiftPlatformUtils::SetMousePosGlobal(x, y);
    }

    std::pair<float, float> Mouse::GetMousePos()
    {
        std::pair<float, float> result;
        result.first = SwiftPlatformUtils::GetMousePosX();
        result.second = SwiftPlatformUtils::GetMousePosY();
        return result;
    }

    Cursor::Cursor(CursorType type)
    {
        m_Cursor = new MacOSCursorWrapper(type);
    }

    void Cursor::Show() const
    {
        static_cast<MacOSCursorWrapper*>(m_Cursor)->m_Cursor.Show();
    }

    void Clipboard::SetText(const std::string& text)
    {
        SwiftPlatformUtils::SetText(text);
        NB_PLATFORM_DEBUG("Set clipboard text: {0}", text);
    }

    std::string Clipboard::GetText()
    {
        return SwiftPlatformUtils::GetText();
    }

    Ref<Window> WindowUtils::GetForegroundWindow()
    {
        int32_t id = SwiftPlatformUtils::GetForegroundWindowID();
        return WindowManager::Get().GetWindowFromID(id);
    }

    std::filesystem::path Files::GetResourcesPath()
    {
        uint32_t size = 0;
        _NSGetExecutablePath(nullptr, &size);

        std::string buffer(size, '\0');
        _NSGetExecutablePath(buffer.data(), &size);

        return std::filesystem::path(buffer).parent_path().parent_path();
    }

    std::filesystem::path Files::GetUserDataPath()
    {
        if (s_AppName.empty()) return GetResourcesPath();
        const char* homedir = getenv("HOME");
        if (!homedir) NB_PLATFORM_CRITICAL("No home folder found");

        std::filesystem::path path = std::filesystem::path(homedir) / "Library" / "Application Support" / s_AppName;
        std::filesystem::create_directories(path);
        return path;
    }

    Theme::ThemeType Theme::GetTheme()
    {
        if (SwiftPlatformUtils::IsDarkTheme()) return ThemeType::Dark;
        return ThemeType::Light;
    }

    bool Graphics::isGraphicsAPISupported(GraphicsAPI api)
    {
        switch (api)
        {
        case GraphicsAPI::Metal3: return true;
        default: return false;
        }
    }
}
