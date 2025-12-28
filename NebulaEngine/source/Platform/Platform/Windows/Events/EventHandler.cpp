#include "EventHandler.h"

#include <CommCtrl.h>
#include <locale>

#include "Platform/Nebula/CoreSystems/Events/KeyEvent.h"
#include "Platform/Nebula/CoreSystems/Events/MouseEvent.h"
#include "Platform/Nebula/CoreSystems/Events/WindowEvents.h"
#include "Platform/Nebula/CoreSystems/Input/KeyCodes.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"
#include "Platform/Platform/Windows/Windows/WindowsWindow.h"

namespace Nebula::Platform
{

    LRESULT EventHandler::SubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,
        DWORD_PTR dwRefData)
    {
        auto* window = reinterpret_cast<Window*>(dwRefData);
        bool CallSubclassWNDProc = true;
        LRESULT result = 0;

        if (window->GetEventCallback())
        {
            switch (msg)
            {
            case WM_CLOSE:
                {
                    WindowClosedEvent event{};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);
                    CallSubclassWNDProc = false;
                    result = 0;
                    break;
                }

            case WM_MOVE:
                {
                    WindowMovedEvent event{LOWORD(lParam), HIWORD(lParam)};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    window->GetProperties().x = LOWORD(lParam);
                    window->GetProperties().y = HIWORD(lParam);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_SIZE:
                {
                    WindowResizedEvent event{LOWORD(lParam), HIWORD(lParam)};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    window->GetProperties().Width = LOWORD(lParam);
                    window->GetProperties().Height = HIWORD(lParam);

                    window->RecalculateHitTest();

                    if (wParam == SIZE_MAXIMIZED)
                    {
                        WindowMaximizedEvent maxEvent{};
                        maxEvent.WindowID = window->GetID();
                        window->GetEventCallback()(maxEvent);

                        window->GetProperties().Maximized = true;
                        window->GetProperties().Minimized = false;
                    }

                    if (wParam == SIZE_MINIMIZED)
                    {
                        WindowMinimizedEvent minEvent{};
                        minEvent.WindowID = window->GetID();
                        window->GetEventCallback()(minEvent);

                        window->GetProperties().Maximized = false;
                        window->GetProperties().Minimized = true;
                    }

                    if (wParam == SIZE_RESTORED)
                    {
                        WindowRestoredEvent restoreEvent{};
                        restoreEvent.WindowID = window->GetID();
                        window->GetEventCallback()(restoreEvent);

                        window->GetProperties().Maximized = false;
                        window->GetProperties().Minimized = false;
                    }

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_SETFOCUS:
                {
                    WindowFocusedEvent event{};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_KILLFOCUS:
                {
                    WindowLostFocusEvent event{};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_DISPLAYCHANGE:
                {
                    MonitorsChangedEvent event{};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_SETTINGCHANGE:
                {
                    if (wParam == SPI_SETWORKAREA)
                    {
                        MonitorsChangedEvent event{};
                        event.WindowID = window->GetID();
                        window->GetEventCallback()(event);
                    }

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                {
                    KeyPressedEvent event{static_cast<KeyboardKey>(wParam), 0};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_KEYUP:
            case WM_SYSKEYUP:
                {
                    KeyReleasedEvent event{static_cast<KeyboardKey>(wParam)};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_CHAR:
                {
                    if (IsWindowUnicode(hwnd))
                    {
                        std::wstring ws{static_cast<wchar_t>(wParam)};
                        std::string s{ws.begin(), ws.end()};
                        char c = s[0];

                        KeyTypedEvent event{c};
                        event.WindowID = window->GetID();
                        window->GetEventCallback()(event);
                    }
                    else
                    {
                        wchar_t character = 0;
                        MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCSTR>(&wParam), 1, &character, 1);
                        std::wstring ws{&character};
                        std::string s{ws.begin(), ws.end()};
                        char c = s[0];

                        KeyTypedEvent event{c};
                        event.WindowID = window->GetID();
                        window->GetEventCallback()(event);
                    }

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_SETCURSOR:
                {
                    // This is required to restore cursor when transitioning from e.g resize borders to client area.
                    if (LOWORD(lParam) == HTCLIENT)
                        return 1;
                    return 0;
                }

            case WM_MOUSEMOVE:
                {
                    if (!window->GetProperties().isMouseHovering)
                    {
                        TRACKMOUSEEVENT tme = {
                            sizeof(tme),
                            TME_LEAVE,
                            hwnd,
                            0
                        };
                        TrackMouseEvent(&tme);
                        window->GetProperties().isMouseHovering = true;

                        MouseEnteredEvent event{};
                        event.WindowID = window->GetID();
                        window->GetEventCallback()(event);
                    }

                    MouseMovedEvent event{static_cast<float>(LOWORD(lParam)), static_cast<float>(HIWORD(lParam))};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_NCMOUSEMOVE:
                {
                    MouseMovedEvent event{static_cast<float>(LOWORD(lParam)), static_cast<float>(HIWORD(lParam))};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_MOUSEHWHEEL:
                {
                    MouseScrolledEvent event{0.f, static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_MOUSEWHEEL:
                {
                    MouseScrolledEvent event{static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA, 0.f};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
            case WM_XBUTTONDOWN:
                {
                    MouseButton pressedButton = MouseButton::Unknown;
                    if (msg == WM_LBUTTONDOWN) pressedButton = MouseButton::Left;
                    if (msg == WM_RBUTTONDOWN) pressedButton = MouseButton::Right;
                    if (msg == WM_MBUTTONDOWN) pressedButton = MouseButton::Middle;
                    if (msg == WM_XBUTTONDOWN) pressedButton = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? MouseButton::X1 : MouseButton::X2;

                    MouseButtonPressedEvent event{pressedButton};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            case WM_XBUTTONUP:
                {
                    MouseButton releasedButton = MouseButton::Unknown;
                    if (msg == WM_LBUTTONUP) releasedButton = MouseButton::Left;
                    if (msg == WM_RBUTTONUP) releasedButton = MouseButton::Right;
                    if (msg == WM_MBUTTONUP) releasedButton = MouseButton::Middle;
                    if (msg == WM_XBUTTONUP) releasedButton = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? MouseButton::X1 : MouseButton::X2;

                    MouseButtonReleasedEvent event{releasedButton};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_MOUSELEAVE:
                {
                    TRACKMOUSEEVENT tme = {
                        sizeof(tme),
                        TME_CANCEL,
                        hwnd,
                        0
                    };
                    TrackMouseEvent(&tme);
                    window->GetProperties().isMouseHovering = false;

                    MouseLeftEvent event{};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

                /**
            case WM_NCHITTEST:
                {
                    HitTest hittest = window->GetWindowHitTest()(LOWORD(lParam), HIWORD(lParam));
                    result = ConvertHitTestResult(hittest);

                    if (result == HTCAPTION) CallSubclassWNDProc = true;
                    else
                    {
                        DefSubclassProc(hwnd, msg, wParam, lParam);
                        CallSubclassWNDProc = false;
                    }
                    break;
                }
                **/

            default: break;
            }
        }

        if (CallSubclassWNDProc) result = DefSubclassProc(hwnd, msg, wParam, lParam);
        return result;
    }

    int32_t EventHandler::ConvertHitTestResult(HitTest hittest)
    {
        switch (hittest)
        {
        case HitTest::Client: return HTCLIENT;
        case HitTest::TitleBar: return HTCAPTION;
        case HitTest::ResizeTopLeft: return HTTOPLEFT;
        case HitTest::ResizeTop: return HTTOP;
        case HitTest::ResizeTopRight: return HTTOPRIGHT;
        case HitTest::ResizeLeft: return HTLEFT;
        case HitTest::ResizeRight: return HTRIGHT;
        case HitTest::ResizeBottomLeft: return HTBOTTOMLEFT;
        case HitTest::ResizeBottom: return HTBOTTOM;
        case HitTest::ResizeBottomRight: return HTBOTTOMRIGHT;
        case HitTest::None: return HTNOWHERE;
        }
    }
}
