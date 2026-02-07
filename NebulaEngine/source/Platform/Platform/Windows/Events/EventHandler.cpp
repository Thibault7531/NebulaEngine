#include "EventHandler.h"

#include <CommCtrl.h>
#include <locale>

#include "Platform/Nebula/CoreSystems/Core/Log.h"
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

            case WM_NCCALCSIZE:
                {
                    if (wParam == TRUE && window->GetProperties().Style == WindowStyle::BorderlessFullscreen)
                    {
                        result = 0;
                        CallSubclassWNDProc = false;
                        break;
                    }
                }

            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                {
                    KeyPressedEvent event{GetNebulaKeycodeFromNativeKeycode(wParam), 0};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_KEYUP:
            case WM_SYSKEYUP:
                {
                    KeyReleasedEvent event{GetNebulaKeycodeFromNativeKeycode(wParam)};
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

                    if (window->GetProperties().isCursorChanged)
                    {
                        SetCursor(LoadCursor(nullptr, IDC_ARROW));
                        window->GetProperties().isCursorChanged = false;
                    }

                    MouseMovedEvent event{static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam))};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_NCMOUSEMOVE:
                {
                    MouseMovedEvent event{static_cast<float>(GET_X_LPARAM(lParam)) - window->GetClientPos().first, static_cast<float>(GET_Y_LPARAM(lParam)) - window->GetClientPos().second};

                    if (window->GetProperties().Style == WindowStyle::BorderlessInteractive)
                    {
                        if (event.GetX() < 4.f && event.GetY() < 9.f)
                        {
                            SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
                            window->GetProperties().isCursorChanged = true;
                        }
                        else if (event.GetX() < 0.f && event.GetY() <= window->GetClientSize().second)
                        {
                            SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
                            window->GetProperties().isCursorChanged = true;
                        }
                        else if (event.GetX() < 10.f && event.GetY() > window->GetClientSize().second)
                        {
                            SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
                            window->GetProperties().isCursorChanged = true;
                        }
                        else if (event.GetX() >= 0.f && event.GetX() <= window->GetClientSize().first && event.GetY() < 4.f)
                        {
                            SetCursor(LoadCursor(nullptr, IDC_SIZENS));
                            window->GetProperties().isCursorChanged = true;
                        }
                        else if (event.GetX() >= 0.f && event.GetX() <= window->GetClientSize().first && event.GetY() >= window->GetClientSize().second)
                        {
                            SetCursor(LoadCursor(nullptr, IDC_SIZENS));
                            window->GetProperties().isCursorChanged = true;
                        }
                        else if (event.GetX() >= window->GetClientSize().first && event.GetY() < 9.f)
                        {
                            SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
                            window->GetProperties().isCursorChanged = true;
                        }
                        else if (event.GetX() >= window->GetClientSize().first && event.GetY() <= window->GetClientSize().second)
                        {
                            SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
                            window->GetProperties().isCursorChanged = true;
                        }
                        else if (event.GetX() >= window->GetClientSize().first && event.GetY() >= window->GetClientSize().second)
                        {
                            SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
                            window->GetProperties().isCursorChanged = true;
                        }
                        else
                        {
                            SetCursor(LoadCursor(nullptr, IDC_ARROW));
                            window->GetProperties().isCursorChanged = false;
                        }
                    }

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_MOUSEHWHEEL:
                {
                    MouseScrolledEvent event{static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA, 0.f};
                    event.WindowID = window->GetID();
                    window->GetEventCallback()(event);

                    CallSubclassWNDProc = true;
                    break;
                }

            case WM_MOUSEWHEEL:
                {
                    MouseScrolledEvent event{0.f, static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA};
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

    KeyboardKey EventHandler::GetNebulaKeycodeFromNativeKeycode(int32_t keycode)
    {
        switch (keycode)
        {
        case VK_BACK: return KeyboardKey::Backspace;
        case VK_TAB: return KeyboardKey::Tab;
        case VK_CLEAR: return KeyboardKey::Clear;
        case VK_RETURN: return KeyboardKey::Return;
        case VK_SHIFT: return KeyboardKey::LeftShift;
        case VK_CONTROL: return KeyboardKey::LeftControl;
        case VK_MENU: return KeyboardKey::LeftGUI;
        case VK_PAUSE: return KeyboardKey::Pause;
        case VK_CAPITAL: return KeyboardKey::CapsLock;
        case VK_ESCAPE: return KeyboardKey::Escape;
        case VK_SPACE: return KeyboardKey::Space;
        case VK_PRIOR: return KeyboardKey::PageUp;
        case VK_NEXT: return KeyboardKey::PageDown;
        case VK_END: return KeyboardKey::End;
        case VK_HOME: return KeyboardKey::Home;
        case VK_LEFT: return KeyboardKey::LeftArrow;
        case VK_UP: return KeyboardKey::UpArrow;
        case VK_RIGHT: return KeyboardKey::RightArrow;
        case VK_DOWN: return KeyboardKey::DownArrow;
        case VK_SELECT: return KeyboardKey::Select;
        case VK_PRINT: return KeyboardKey::ACPrint;
        case VK_EXECUTE: return KeyboardKey::Execute;
        case VK_SNAPSHOT: return KeyboardKey::PrintScreen;
        case VK_INSERT: return KeyboardKey::Insert;
        case VK_DELETE: return KeyboardKey::Delete;
        case VK_HELP: return KeyboardKey::Help;
        case 0x30: return KeyboardKey::Num0;
        case 0x31: return KeyboardKey::Num1;
        case 0x32: return KeyboardKey::Num2;
        case 0x33: return KeyboardKey::Num3;
        case 0x34: return KeyboardKey::Num4;
        case 0x35: return KeyboardKey::Num5;
        case 0x36: return KeyboardKey::Num6;
        case 0x37: return KeyboardKey::Num7;
        case 0x38: return KeyboardKey::Num8;
        case 0x39: return KeyboardKey::Num9;
        case 0x41: return KeyboardKey::A;
        case 0x42: return KeyboardKey::B;
        case 0x43: return KeyboardKey::C;
        case 0x44: return KeyboardKey::D;
        case 0x45: return KeyboardKey::E;
        case 0x46: return KeyboardKey::F;
        case 0x47: return KeyboardKey::G;
        case 0x48: return KeyboardKey::H;
        case 0x49: return KeyboardKey::I;
        case 0x4A: return KeyboardKey::J;
        case 0x4B: return KeyboardKey::K;
        case 0x4C: return KeyboardKey::L;
        case 0x4D: return KeyboardKey::M;
        case 0x4E: return KeyboardKey::N;
        case 0x4F: return KeyboardKey::O;
        case 0x50: return KeyboardKey::P;
        case 0x51: return KeyboardKey::Q;
        case 0x52: return KeyboardKey::R;
        case 0x53: return KeyboardKey::S;
        case 0x54: return KeyboardKey::T;
        case 0x55: return KeyboardKey::U;
        case 0x56: return KeyboardKey::V;
        case 0x57: return KeyboardKey::W;
        case 0x58: return KeyboardKey::X;
        case 0x59: return KeyboardKey::Y;
        case 0x5A: return KeyboardKey::Z;
        case VK_LWIN: return KeyboardKey::LeftGUI;
        case VK_RWIN: return KeyboardKey::RightGUI;
        case VK_APPS: return KeyboardKey::Application;
        case VK_SLEEP: return KeyboardKey::Sleep;
        case VK_NUMPAD0: return KeyboardKey::Keypad0;
        case VK_NUMPAD1: return KeyboardKey::Keypad1;
        case VK_NUMPAD2: return KeyboardKey::Keypad2;
        case VK_NUMPAD3: return KeyboardKey::Keypad3;
        case VK_NUMPAD4: return KeyboardKey::Keypad4;
        case VK_NUMPAD5: return KeyboardKey::Keypad5;
        case VK_NUMPAD6: return KeyboardKey::Keypad6;
        case VK_NUMPAD7: return KeyboardKey::Keypad7;
        case VK_NUMPAD8: return KeyboardKey::Keypad8;
        case VK_NUMPAD9: return KeyboardKey::Keypad9;
        case VK_MULTIPLY: return KeyboardKey::KeypadMultiply;
        case VK_ADD: return KeyboardKey::KeypadPlus;
        case VK_SEPARATOR: return KeyboardKey::Separator;
        case VK_SUBTRACT: return KeyboardKey::KeypadMinus;
        case VK_DECIMAL: return KeyboardKey::KeypadDecimal;
        case VK_DIVIDE: return KeyboardKey::KeypadDivide;
        case VK_F1: return KeyboardKey::F1;
        case VK_F2: return KeyboardKey::F2;
        case VK_F3: return KeyboardKey::F3;
        case VK_F4: return KeyboardKey::F4;
        case VK_F5: return KeyboardKey::F5;
        case VK_F6: return KeyboardKey::F6;
        case VK_F7: return KeyboardKey::F7;
        case VK_F8: return KeyboardKey::F8;
        case VK_F9: return KeyboardKey::F9;
        case VK_F10: return KeyboardKey::F10;
        case VK_F11: return KeyboardKey::F11;
        case VK_F12: return KeyboardKey::F12;
        case VK_F13: return KeyboardKey::F13;
        case VK_F14: return KeyboardKey::F14;
        case VK_F15: return KeyboardKey::F15;
        case VK_F16: return KeyboardKey::F16;
        case VK_F17: return KeyboardKey::F17;
        case VK_F18: return KeyboardKey::F18;
        case VK_F19: return KeyboardKey::F19;
        case VK_F20: return KeyboardKey::F20;
        case VK_F21: return KeyboardKey::F21;
        case VK_F22: return KeyboardKey::F22;
        case VK_F23: return KeyboardKey::F23;
        case VK_F24: return KeyboardKey::F24;
        case VK_NUMLOCK: return KeyboardKey::NumLock;
        case VK_SCROLL: return KeyboardKey::ScrollLock;
        case VK_LSHIFT: return KeyboardKey::LeftShift;
        case VK_RSHIFT: return KeyboardKey::RightShift;
        case VK_LCONTROL: return KeyboardKey::LeftControl;
        case VK_RCONTROL: return KeyboardKey::RightControl;
        case VK_LMENU: return KeyboardKey::LeftAlt;
        case VK_RMENU: return KeyboardKey::RightAlt;
        case VK_BROWSER_BACK: return KeyboardKey::ACBack;
        case VK_BROWSER_FORWARD: return KeyboardKey::ACForward;
        case VK_BROWSER_REFRESH: return KeyboardKey::ACRefresh;
        case VK_BROWSER_STOP: return KeyboardKey::ACStop;
        case VK_BROWSER_SEARCH: return KeyboardKey::ACSearch;
        case VK_BROWSER_FAVORITES: return KeyboardKey::ACFavorites;
        case VK_BROWSER_HOME: return KeyboardKey::ACHome;
        case VK_VOLUME_MUTE: return KeyboardKey::Mute;
        case VK_VOLUME_DOWN: return KeyboardKey::VolumeDown;
        case VK_VOLUME_UP: return KeyboardKey::VolumeUp;
        case VK_MEDIA_NEXT_TRACK: return KeyboardKey::AudioNextTrack;
        case VK_MEDIA_PREV_TRACK: return KeyboardKey::AudioPreviousTrack;
        case VK_MEDIA_STOP: return KeyboardKey::AudioStop;
        case VK_MEDIA_PLAY_PAUSE: return KeyboardKey::AudioPlayPause;
        case VK_LAUNCH_MEDIA_SELECT: return KeyboardKey::AudioSelect;
        default: return KeyboardKey::Unknown;
        }
    }
}
