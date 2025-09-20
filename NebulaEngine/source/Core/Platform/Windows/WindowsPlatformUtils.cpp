#include <commdlg.h>
#include <ranges>
#include <shcore.h>
#include <shellscalingapi.h>

#include "Nebula/CoreSystems/Application/Application.h"
#include "Nebula/CoreSystems/Windows/WindowManager.h"
#include "Nebula/Utils/PlatformUtils.h"

typedef HRESULT(WINAPI* PFN_GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);

namespace Nebula
{

    std::string FileDialogs::OpenFile(const std::vector<FileType>& fileTypes)
    {
        // TODO: Update to common item dialog
        // TODO: Run in seperate thread
        char filter[260] = { 0 };
        ConvertFilter(fileTypes, filter);
        const auto hwnd = static_cast<HWND>(WindowManager::Get().GetMainWindow()->GetNativeWindow());
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
        const auto hwnd = static_cast<HWND>(WindowManager::Get().GetMainWindow()->GetNativeWindow());
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

    BOOL CALLBACK GetDisplay(HMONITOR monitor, HDC, LPRECT, LPARAM lparam)
    {
        std::vector<Displays::display>* displays = reinterpret_cast<std::vector<Displays::display>*>(lparam);

        MONITORINFOEX monitorInfo;
        monitorInfo.cbSize = sizeof(monitorInfo);
        GetMonitorInfo(monitor, &monitorInfo);

        Displays::display display;
        display.x = monitorInfo.rcMonitor.left;
        display.y = monitorInfo.rcMonitor.top;
        display.width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        display.height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
        display.workX = monitorInfo.rcWork.left;
        display.workY = monitorInfo.rcWork.top;
        display.workWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
        display.workHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

        UINT xdpi = 96, ydpi = 96;
        {
            // For some reason the GetDpiForMonitor function is not linked and needs to be imported manually. F*** Microsoft
            static HINSTANCE shcore_dll = ::LoadLibraryA("shcore.dll"); // Reference counted per-process
            static PFN_GetDpiForMonitor GetDpiForMonitorFn = nullptr;
            if (shcore_dll != nullptr)
                GetDpiForMonitorFn = (PFN_GetDpiForMonitor)::GetProcAddress(shcore_dll, "GetDpiForMonitor");
            if (GetDpiForMonitorFn != nullptr)
            {
                GetDpiForMonitorFn((HMONITOR)monitor, MDT_EFFECTIVE_DPI, &xdpi, &ydpi);
                display.dpi = xdpi / 96.0f;
            }
        }

        display.nativeDisplay = monitor;

        display.primary = monitorInfo.dwFlags & MONITORINFOF_PRIMARY;

        if (display.primary) displays->insert(displays->begin(), display);
        else displays->push_back(display);

        return TRUE;
    }

    std::vector<Displays::display> Displays::GetDisplays()
    {
        std::vector<display> displays;

        EnumDisplayMonitors(nullptr, nullptr, GetDisplay, reinterpret_cast<LPARAM>(&displays));

        return displays;
    }

    Ref<Window> WindowUtils::GetForegroundWindow()
    {
        HWND hwnd = ::GetForegroundWindow();
        return WindowManager::Get().GetWindow(hwnd);
    }

    void Mouse::ShowCursor()
    {
        ::ShowCursor(TRUE);
    }

    void Mouse::HideCursor()
    {
        ::ShowCursor(FALSE);
    }

    void Mouse::CaptureMouse(const bool enable)
    {
        SetCapture(enable ? static_cast<HWND>(WindowManager::Get().GetMainWindow()->GetNativeWindow()) : nullptr);
    }

    void Mouse::SetMousePosGlobal(int x, int y)
    {
        SetCursorPos(x, y);
    }

    std::pair<float, float> Mouse::GetMousePos()
    {
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        return { cursorPos.x, cursorPos.y };
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

    Ref<Window> Keyboard::GetCapturingWindow()
    {
        if (HWND window = GetFocus())
        {
            return WindowManager::Get().GetWindow(GetWindowLongPtr(window, GWLP_ID));
        }
        return nullptr;
    }

    int Keyboard::GetScancodeFromKeycode(KeyboardKey keycode)
    {
        return MapVirtualKeyA(static_cast<UINT>(keycode), MAPVK_VK_TO_VSC);
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
}
