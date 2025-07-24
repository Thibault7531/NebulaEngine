#include <commdlg.h>
#include <ranges>
#include <SDL.h>

#include "Nebula/CoreSystems/Application/Application.h"
#include "Nebula/CoreSystems/Windows/WindowManager.h"
#include "Nebula/Utils/PlatformUtils.h"

namespace Nebula
{

    std::string FileDialogs::OpenFile(const std::vector<FileType>& fileTypes)
    {
        // TODO: Update to common item dialog
        // TODO: Run in seperate thread
        char filter[260] = { 0 };
        ConvertFilter(fileTypes, filter);
        SDL_PropertiesID props = SDL_GetWindowProperties(static_cast<SDL_Window*>(WindowManager::Get().GetMainWindow()->GetNativeWindow()));
        const auto hwnd = static_cast<HWND>(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };

        ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
        ofn.lStructSize = sizeof(OPENFILENAMEA);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        if (GetOpenFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }
        return {};
    }

    std::string FileDialogs::SaveFile(const char* filter)
    {
        // TODO: Update to common item dialog
        // TODO: Run in seperate thread
        SDL_PropertiesID props = SDL_GetWindowProperties(static_cast<SDL_Window*>(WindowManager::Get().GetMainWindow()->GetNativeWindow()));
        const auto hwnd = static_cast<HWND>(SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
        std::string extension = filter + strlen(filter) + 1;
        std::string defExt = extension.substr(2, extension.find_first_of(';'));
        OPENFILENAMEA ofn;
        CHAR szFile[260] = { 0 };

        ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
        ofn.lStructSize = sizeof(OPENFILENAMEA);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = filter;
        ofn.nFilterIndex = 1;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        ofn.lpstrDefExt = defExt.c_str();
        if (GetSaveFileNameA(&ofn) == TRUE)
        {
            return ofn.lpstrFile;
        }
        return {};
    }

    void FileDialogs::ConvertFilter(const std::vector<FileType>& fileTypes, char* filter)
    {
        int filterLength = 0;
        for (auto filetype : fileTypes)
        {
            switch (filetype)
            {
            case FileType::NebulaScene:
                memcpy_s(filter + filterLength, 36, "Nebula Scene (*.nbscene)\0*.nbscene\0", 36);
            }
        }
    }

    std::vector<Displays::display> Displays::GetDisplays()
    {
        std::vector<display> displays;
        int display_count;
        SDL_DisplayID* display_ids = SDL_GetDisplays(&display_count);

        for (int n = 0; n < display_count; n++)
        {
            SDL_DisplayID display_id = display_ids[n];
            display display;

            SDL_Rect r;

            SDL_GetDisplayBounds(display_id, &r);
            display.x = r.x;
            display.y = r.y;
            display.width = r.w;
            display.height = r.h;

            SDL_GetDisplayUsableBounds(display_id, &r);
            display.workX = r.x;
            display.workY = r.y;
            display.workWidth = r.w;
            display.workHeight = r.h;

            display.dpi = SDL_GetDisplayContentScale(display_id);

            displays.push_back(display);
        }

        SDL_free(display_ids);
        return displays;
    }

    void Mouse::ShowCursor()
    {
        SDL_ShowCursor();
    }

    void Mouse::HideCursor()
    {
        SDL_HideCursor();
    }

    void Mouse::CaptureMouse(const bool enable)
    {
        SDL_CaptureMouse(enable);
    }

    void Mouse::SetMousePosGlobal(int x, int y)
    {
        SDL_WarpMouseGlobal(x, y);
    }

    std::pair<float, float> Mouse::GetMousePos()
    {
        float x, y;
        SDL_GetGlobalMouseState(&x, &y);
        return { x, y };
    }

    Cursor::Cursor(CursorType type)
    {
        switch (type)
        {
        case CursorType::Arrow:
            m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
            return;

        case CursorType::TextInput:
            m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
            return;

        case CursorType::Move:
            m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
            return;

        case CursorType::ResizeNS:
            m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
            return;

        case CursorType::ResizeEW:
            m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
            return;

        case CursorType::ResizeNESW:
            m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
            return;

        case CursorType::ResizeNWSE:
            m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
            return;

        case CursorType::Hand:
            m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
            return;

        case CursorType::NotAllowed:
            m_Cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);
            return;
        }
    }

    void Cursor::Show()
    {
        SDL_SetCursor(static_cast<SDL_Cursor*>(m_Cursor));
    }

    Ref<Window> Keyboard::GetCapturingWindow()
    {
        if (SDL_WindowID windowID = SDL_GetWindowID(SDL_GetKeyboardFocus()))
        {
            return WindowManager::Get().GetWindow(windowID);
        }
        return nullptr;
    }

    int Keyboard::GetScancodeFromKeycode(int keycode)
    {
        return SDL_GetScancodeFromKey(keycode, nullptr);
    }

    void Clipboard::SetText(const std::string& text)
    {
        SDL_SetClipboardText(text.c_str());
    }

    std::string Clipboard::GetText()
    {
        return SDL_GetClipboardText();
    }
}
