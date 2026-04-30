#include "Platform/Nebula/CoreSystems/Core/PlatformUtils.h"

#include <Windows.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.ViewManagement.h>

#include "Platform/Nebula/CoreSystems/Input/KeyCodes.h"
#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"

namespace Nebula::Platform
{
    std::string Files::s_AppName = "";

    std::vector<Display> Displays::GetDisplays()
    {
        winrt::Windows::Foundation::Collections::IVectorView<winrt::Microsoft::UI::Windowing::DisplayArea> displays = winrt::Microsoft::UI::Windowing::DisplayArea::FindAll();
        std::vector<Display> result;

        for (winrt::Microsoft::UI::Windowing::DisplayArea display : displays)
        {
            Display d;
            d.x = display.OuterBounds().X;
            d.y = display.OuterBounds().Y;
            d.width = display.OuterBounds().Width;
            d.height = display.OuterBounds().Height;
            d.workX = display.WorkArea().X;
            d.workY = display.WorkArea().Y;
            d.workWidth = display.WorkArea().Width;
            d.workHeight = display.WorkArea().Height;
            d.primary = display.IsPrimary();

            result.push_back(d);
        }

        return result;
    }

    void Mouse::ShowCursor()
    {
        ::ShowCursor(TRUE);
    }

    void Mouse::HideCursor()
    {
        ::ShowCursor(FALSE);
    }

    void Mouse::SetMousePosGlobal(int32_t x, int32_t y)
    {
        SetCursorPos(x, y);
    }

    std::pair<float, float> Mouse::GetMousePos()
    {
        POINT pos;
        GetCursorPos(&pos);
        return { pos.x, pos.y };
    }

    Cursor::Cursor(CursorType type)
    {
        switch (type)
        {
        case CursorType::Arrow:
            m_Cursor = LoadCursor(nullptr, IDC_ARROW);
            return;

        case CursorType::TextInput:
            m_Cursor = LoadCursor(nullptr, IDC_IBEAM);
            return;

        case CursorType::Move:
            m_Cursor = LoadCursor(nullptr, IDC_SIZEALL);
            return;

        case CursorType::ResizeNS:
            m_Cursor = LoadCursor(nullptr, IDC_SIZENS);
            return;

        case CursorType::ResizeEW:
            m_Cursor = LoadCursor(nullptr, IDC_SIZEWE);
            return;

        case CursorType::ResizeNESW:
            m_Cursor = LoadCursor(nullptr, IDC_SIZENESW);
            return;

        case CursorType::ResizeNWSE:
            m_Cursor = LoadCursor(nullptr, IDC_SIZENWSE);
            return;

        case CursorType::Hand:
            m_Cursor = LoadCursor(nullptr, IDC_HAND);
            return;

        case CursorType::NotAllowed:
            m_Cursor = LoadCursor(nullptr, IDC_NO);
            return;
        }
    }

    void Cursor::Show() const
    {
        SetCursor(static_cast<HCURSOR>(m_Cursor));
    }

    void Clipboard::SetText(const std::string& text)
    {
        OpenClipboard(nullptr);

        SetClipboardData(CF_TEXT, (HANDLE)text.c_str());
        SetClipboardData(CF_UNICODETEXT, (HANDLE)text.c_str());

        CloseClipboard();
    }

    std::string Clipboard::GetText()
    {
        HANDLE h;

        if (!OpenClipboard(nullptr))
            return "";

        h = GetClipboardData(CF_TEXT);

        std::string text;
        strcpy_s(text.data(), 1024, static_cast<const char*>(h));

        CloseClipboard();

        return text;
    }

    Ref<Window> WindowUtils::GetForegroundWindow()
    {
        HWND hwnd = ::GetForegroundWindow();
        return WindowManager::Get().GetWindowFromHandle(hwnd);
    }

    std::filesystem::path Files::GetResourcesPath()
    {
        char buffer[MAX_PATH];
        GetModuleFileName(nullptr, buffer, MAX_PATH);
        return std::filesystem::path(buffer).parent_path();
    }

    std::filesystem::path Files::GetUserDataPath()
    {
        if (s_AppName.empty()) return GetResourcesPath();
        std::filesystem::path path = std::filesystem::path(getenv("APPDATA")) / s_AppName;
        std::filesystem::create_directories(path);
        return path;
    }

    Theme::ThemeType Theme::GetTheme()
    {
        auto settings = winrt::Windows::UI::ViewManagement::UISettings();
        auto foreground = settings.GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::Foreground);
        bool isDark = (((5 * foreground.G) + (2 * foreground.R) + foreground.B) > (8 * 128));

        return isDark ? ThemeType::Dark : ThemeType::Light;
    }
}
