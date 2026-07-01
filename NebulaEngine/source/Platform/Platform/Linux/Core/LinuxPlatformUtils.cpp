#include "Platform//Nebula/CoreSystems/Core/PlatformUtils.h"

#include <SDL3/SDL.h>

#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"

namespace Nebula::Platform
{
    std::string Files::s_AppName = "";
    Theme::ThemeType Theme::GetTheme()
    {
        SDL_SystemTheme theme = SDL_GetSystemTheme();
        switch (theme)
        {
            case SDL_SYSTEM_THEME_LIGHT: return ThemeType::Light;
            case SDL_SYSTEM_THEME_DARK:  return ThemeType::Dark;
            default: return ThemeType::Light;
        }
    }

    std::vector<Display> Displays::GetDisplays()
    {
        int count;
        SDL_DisplayID* displays = SDL_GetDisplays(&count);
        std::vector<Display> result;

        for (int i = 0; i < count; i++)
        {
            Display currentDisplay;

            SDL_Rect bounds;
            SDL_GetDisplayBounds(displays[i], &bounds);
            currentDisplay.x = bounds.x;
            currentDisplay.y = bounds.y;
            currentDisplay.width = bounds.w;
            currentDisplay.height = bounds.h;

            SDL_GetDisplayUsableBounds(displays[i], &bounds);
            currentDisplay.workX = bounds.x;
            currentDisplay.workY = bounds.y;
            currentDisplay.workWidth = bounds.w;
            currentDisplay.workHeight = bounds.h;

            if (i == 0) currentDisplay.primary = true;

            result.push_back(currentDisplay);
        }

        return result;
    }

    void Mouse::ShowCursor()
    {
        SDL_ShowCursor();
    }

    void Mouse::HideCursor()
    {
        SDL_HideCursor();
    }

    void Mouse::SetMousePosGlobal(int32_t x, int32_t y)
    {
        SDL_WarpMouseGlobal(x, y);
    }

    std::pair<float, float> Mouse::GetMousePos()
    {
        float x, y;
        SDL_GetGlobalMouseState(&x, &y);
        return {x, y};
    }

    Cursor::Cursor(CursorType type)
    {
        switch (type)
        {
        case CursorType::Arrow:
            {
                m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
                break;
            }

        case CursorType::TextInput:
            {
                m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
                break;
            }

        case CursorType::Move:
            {
                m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
                break;
            }

        case CursorType::ResizeNS:
            {
                m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
                break;
            }

        case CursorType::ResizeEW:
            {
                m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
                break;
            }

        case CursorType::ResizeNESW:
            {
                m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
                break;
            }

        case CursorType::ResizeNWSE:
            {
                m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
                break;
            }

        case CursorType::Hand:
            {
                m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
                break;
            }

        case CursorType::NotAllowed:
            {
                m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);
                break;
            }
        }
    }

    void Cursor::Show() const
    {
        SDL_SetCursor(static_cast<SDL_Cursor*>(m_Cursor));
    }

    void Clipboard::SetText(const std::string& text)
    {
        SDL_SetClipboardText(text.c_str());
    }

    std::string Clipboard::GetText()
    {
        return SDL_GetClipboardText();
    }

    Ref<Window> WindowUtils::GetForegroundWindow()
    {
        return WindowManager::Get().GetWindowFromHandle(SDL_GetKeyboardFocus());
    }

    std::filesystem::path Files::GetResourcesPath()
    {
        return std::filesystem::canonical("/proc/self/exe").parent_path();
    }

    std::filesystem::path Files::GetUserDataPath()
    {
        if (s_AppName.empty()) return GetResourcesPath();

        const char* xdg = getenv("XDG_DATA_HOME");
        std::filesystem::path path;
        if (xdg) path = std::filesystem::path(xdg) / s_AppName;
        else path = std::filesystem::path(getenv("HOME")) / ".local" / "share" / s_AppName;
        std::filesystem::create_directories(path);
        return path;
    }

    bool Graphics::isGraphicsAPISupported(GraphicsAPI api)
    {
        switch (api)
        {
            case GraphicsAPI::OpenGL410: return true;
            case GraphicsAPI::OpenGL460: return true;
            default: return false;
        }
    }
}
