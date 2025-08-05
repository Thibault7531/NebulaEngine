#include "WindowsWindow.h"

#include <dwmapi.h>
#include <windowsx.h>

#include "Nebula/CoreSystems/Windows/WindowManager.h"
#include "Nebula/Events/ApplicationEvent.h"
#include "Nebula/Events/KeyEvent.h"
#include "Nebula/Events/MouseEvent.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Nebula
{
    bool WindowsWindow::s_ClassRegistered = false;
    ATOM WindowsWindow::s_WindowClassAtom = 0;
    WNDCLASSEXA WindowsWindow::s_WindowClass = {};
    HINSTANCE WindowsWindow::s_hInstance = nullptr;

    WindowsWindow::WindowsWindow(const WindowProps& props)
    {
        NB_PROFILE_FUNCTION();

        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;
        m_Data.Border = props.Border;
        m_Data.VSync = false;
        m_Data.Maximized = props.Maximized;
        m_Data.Fullscreen = props.Fullscreen;
        m_Data.HighDPI = props.HighDPI;
        m_Data.API = props.API;

        if (!s_ClassRegistered) RegisterWindowClass();

        m_Style = WS_SYSMENU | WS_MINIMIZEBOX;
        m_ExStyle = 0;
        RECT windowSize = {
            .right = static_cast<LONG>(m_Data.Width),
            .bottom = static_cast<LONG>(m_Data.Height),
        };

        if (!props.isImGui) m_Style |= WS_CAPTION | WS_THICKFRAME;
        if (props.Border)
        {
            m_Style |= WS_OVERLAPPED;
            m_ExStyle |= WS_EX_OVERLAPPEDWINDOW;
            AdjustWindowRectEx(&windowSize, m_Style, false, m_ExStyle);
        }
        else
        {
            m_Style |= WS_POPUP;
        }
        if (props.isResizable) m_Style |= WS_MAXIMIZEBOX;
        if (props.Maximized) m_Style |= WS_MAXIMIZE;
        if (props.isUtility) m_ExStyle |= WS_EX_TOOLWINDOW;
        if (props.AlwaysOnTop) m_ExStyle |= WS_EX_TOPMOST;

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        m_Window = CreateWindowEx(m_ExStyle, MAKEINTATOM(s_WindowClassAtom), m_Data.Title.c_str(), m_Style, props.x == -INT_MAX ? screenWidth/2.f - (windowSize.right - windowSize.left)/2.f : props.x, props.y == -INT_MAX ? screenHeight/2.f - (windowSize.bottom - windowSize.top)/2.f : props.y, windowSize.right - windowSize.left, windowSize.bottom - windowSize.top, nullptr, nullptr, s_hInstance, &m_Data);

        if (!m_Window)
        {
            WCHAR wszMsgBuff[512];
            DWORD dwChars;

            dwChars = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, GetLastError(), 0, wszMsgBuff, 512, nullptr);

            std::wstring ws(wszMsgBuff);
            std::string s(ws.begin(), ws.end());
            NB_CORE_ERROR("Failed to create window: {}", s);
        }

        SetProp(m_Window, "NebulaWindowProps", &m_Data);
        SetWindowPos(m_Window, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
        ShowWindow(m_Window, SW_SHOWDEFAULT);

        if (m_Data.API == RendererAPI::API::OpenGL) m_Context = CreateRef<OpenGLContext>(m_Window);
        m_Context->Init();
    }

    WindowsWindow::~WindowsWindow()
    {
        m_Context.reset();
        DestroyWindow(m_Window);
    }

    void WindowsWindow::OnUpdate()
    {
        m_Context->SetCurrent();
        m_Context->SwapBuffers();

        MSG msg{};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void WindowsWindow::Shutdown()
    {
        PostQuitMessage(0);
    }

    void WindowsWindow::SwapBuffers() const
    {
        ::SwapBuffers(GetDC(m_Window));
    }

    void WindowsWindow::SetWindowHitTest(std::function<HitTest(int, int)> callback)
    {
        m_Data.HitTestCallback = callback;
    }

    void WindowsWindow::SetSize(int w, int h)
    {
        m_Data.Width = w;
        m_Data.Height = h;
        RECT windowSize = {
            .left = 0,
            .top = 0,
            .right = static_cast<LONG>(w),
            .bottom = static_cast<LONG>(h),
        };
        if (m_Data.Border) AdjustWindowRectEx(&windowSize, m_Style, false, m_ExStyle);
        SetWindowPos(m_Window, nullptr, 0, 0, windowSize.right - windowSize.left, windowSize.bottom - windowSize.top, SWP_NOZORDER | SWP_NOMOVE);
    }

    std::pair<unsigned int, unsigned int> WindowsWindow::GetPixelSize() const
    {
        RECT clientSize;
        GetClientRect(m_Window, &clientSize);
        return {clientSize.right - clientSize.left, clientSize.bottom - clientSize.top};
    }

    std::pair<int, int> WindowsWindow::GetPos() const
    {
        RECT clientPos;
        GetClientRect(m_Window, &clientPos);
        MapWindowPoints(m_Window, nullptr, reinterpret_cast<LPPOINT>(&clientPos), 2);
        return {clientPos.left, clientPos.top};
    }

    void WindowsWindow::SetPos(int x, int y)
    {
        SetWindowPos(m_Window, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }

    void WindowsWindow::SetMousePos(int x, int y) const
    {
        RECT window;
        GetWindowRect(m_Window, &window);
        AdjustWindowRectEx(&window, m_Style, false, m_ExStyle);
        SetCursorPos(window.left + x, window.top + y);
    }

    void WindowsWindow::SetVSync(bool enabled)
    {
        m_Context->SetVSync(enabled);
        m_Data.VSync = enabled;
    }

    void WindowsWindow::SetFullscreen(bool enabled)
    {
        if (enabled)
        {
            DEVMODE dmScreenSettings;
            memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));

            EnumDisplaySettingsEx(nullptr, ENUM_CURRENT_SETTINGS, &dmScreenSettings, 0);

            NB_CORE_ASSERT(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL, "Failed to set fullscreen mode!")
        } else {
            ChangeDisplaySettings(nullptr, 0);
        }
    }

    void WindowsWindow::SetMaximized(bool enabled)
    {
        if (enabled)
        {
            m_Data.Maximized = true;
            ShowWindow(m_Window, SW_MAXIMIZE);
        } else {
            m_Data.Maximized = false;
            ShowWindow(m_Window, SW_RESTORE);
        }
    }

    void WindowsWindow::Minimize()
    {
        ShowWindow(m_Window, SW_MINIMIZE);
    }

    bool WindowsWindow::IsFocused() const
    {
        return GetFocus() == m_Window;
    }

    void WindowsWindow::SetTitle(const std::string& title)
    {
        SetWindowText(m_Window, title.c_str());
        m_Data.Title = title;
    }

    void WindowsWindow::SetAlpha(float alpha)
    {
        SetLayeredWindowAttributes(m_Window, 0, alpha, LWA_ALPHA);
    }

    void WindowsWindow::SetParentWindow(Ref<Window> parent) const
    {
        SetParent(m_Window, static_cast<HWND>(parent->GetNativeWindow()));
    }

    void WindowsWindow::Show() const
    {
        ShowWindow(m_Window, SW_SHOW);
    }

    void WindowsWindow::Hide() const
    {
        ShowWindow(m_Window, SW_HIDE);
    }

    void WindowsWindow::Raise() const
    {
        SetWindowPos(m_Window, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }

    void WindowsWindow::Flash() const
    {
        FLASHWINFO flashInfo = {
            .cbSize = sizeof(FLASHWINFO),
            .hwnd = m_Window,
            .dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG,
            .uCount = 0,
            .dwTimeout = 0,
        };
        FlashWindowEx(&flashInfo);
    }

    void WindowsWindow::SetCurrentContext()
    {
        m_Context->SetCurrent();
    }

    void WindowsWindow::SetID(int ID)
    {
        m_WindowID = ID;
    }

    LRESULT WindowsWindow::OnEvent(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        auto* data = static_cast<WindowData*>(GetProp(hwnd, "NebulaWindowProps"));
        Ref<Window> window = WindowManager::Get().GetWindow(hwnd);

        switch (uMsg)
        {
        case WM_CREATE: return 0;
        case WM_NCCREATE:
            {
                auto userdata = reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams;
                SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(userdata));

                RECT rcClient;
                GetWindowRect(hwnd, &rcClient);

                SetWindowPos(hwnd, nullptr, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

                DefWindowProc(hwnd, uMsg, wParam, lParam);
                return TRUE;
            }

        case WM_ACTIVATE:
            {
                MARGINS margins{1, 1, 1, 1};

                HRESULT hr = DwmExtendFrameIntoClientArea(hwnd, &margins);
                SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

                if (!SUCCEEDED(hr)) NB_CORE_DEBUG("Failed to extend frame into client area!");

                if (LOWORD(wParam) == WA_ACTIVE)
                {
                    if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                    {
                        WindowFocusedEvent event{};
                        event.Window = window.get();
                        data->EventCallback(event);
                    }
                }
                else if (LOWORD(wParam) == WA_INACTIVE)
                {
                    if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                    {
                        WindowLostFocusEvent event{};
                        event.Window = window.get();
                        data->EventCallback(event);
                    }
                }

                DefWindowProc(hwnd, uMsg, wParam, lParam);
                return 0;
            }

        case WM_NCACTIVATE:
            {
                DefWindowProc(hwnd, uMsg, wParam, lParam);
                return 1;
            }

        case WM_NCCALCSIZE:
            {
                auto windowdata = reinterpret_cast<WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
                if (wParam == TRUE && !windowdata->Border)
                {
                    auto* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);

                    WINDOWPLACEMENT placement{};
                    if (!GetWindowPlacement(hwnd, &placement)) return 0;

                    if (data != nullptr && placement.showCmd == SW_MAXIMIZE)
                    {
                        auto monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
                        if (!monitor) return 0;

                        MONITORINFO monitorInfo{};
                        monitorInfo.cbSize = sizeof(monitorInfo);
                        if (!GetMonitorInfoW(monitor, &monitorInfo)) return 0;

                        params->rgrc[0] = monitorInfo.rcWork;
                    }

                    return 0;
                }
                break;
            }

        case WM_SIZE:
            {
                if (wParam == SIZE_MAXIMIZED)
                {
                    if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                    {
                        data->Maximized = true;
                        data->Minimized = false;

                        WindowMaximizeEvent event{};
                        event.Window = window.get();
                        data->EventCallback(event);
                    }
                }

                if (wParam == SIZE_MINIMIZED)
                {
                    if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                    {
                        data->Maximized = false;
                        data->Minimized = true;

                        WindowMinimizeEvent event{};
                        event.Window = window.get();
                        data->EventCallback(event);
                    }
                }

                if (wParam == SIZE_RESTORED)
                {
                    if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                    {
                        data->Maximized = false;
                        data->Minimized = false;

                        WindowRestoredEvent event{};
                        event.Window = window.get();
                        data->EventCallback(event);
                    }
                }

                if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                {
                    data->Width = LOWORD(lParam);
                    data->Height = HIWORD(lParam);

                    WindowResizeEvent event(data->Width, data->Height);
                    event.Window = window.get();
                    data->EventCallback(event);
                }

                DefWindowProc(hwnd, uMsg, wParam, lParam);
                return 0;
            }

        case WM_SIZING:
            {
                DefWindowProc(hwnd, uMsg, wParam, lParam);
                return TRUE;
            }

        case WM_MOVE:
            {
                if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                {
                    WindowMovedEvent event{LOWORD(lParam), HIWORD(lParam)};
                    event.Window = window.get();
                    data->EventCallback(event);
                }

                return 0;
            }

        case WM_MOVING:
            {
                DefWindowProc(hwnd, uMsg, wParam, lParam);
                return 0;
            }

        case WM_SETFOCUS:
            {
                if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                {
                    WindowFocusedEvent event{};
                    event.Window = window.get();
                    data->EventCallback(event);
                }

                return 0;
            }

        case WM_KILLFOCUS:
            {
                if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                {
                    WindowLostFocusEvent event{};
                    event.Window = window.get();
                    data->EventCallback(event);
                }

                return 0;
            }

        case WM_CLOSE:
            {
                if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                {
                    WindowCloseEvent event{};
                    event.Window = window.get();
                    data->EventCallback(event);
                }

                return 0;
            }

        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
            {
                if (data == nullptr) return 0;
                const int area = (uMsg == WM_MOUSEMOVE) ? 1 : 2;
                if (data->MouseTrackedArea != area)
                {
                    TRACKMOUSEEVENT tme_cancel = { sizeof(tme_cancel), TME_CANCEL, hwnd, 0 };
                    TRACKMOUSEEVENT tme_track = { sizeof(tme_track), static_cast<DWORD>((area == 2) ? (TME_LEAVE | TME_NONCLIENT) : TME_LEAVE), hwnd, 0 };
                    if (data->MouseTrackedArea != 0) TrackMouseEvent(&tme_cancel);
                    TrackMouseEvent(&tme_track);
                    data->MouseTrackedArea = area;
                }

                if (area == 1 && (data->MouseTrackedArea != 0))
                {
                    if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                    {
                        MouseEnteredEvent event{};
                        event.Window = window.get();
                        data->EventCallback(event);
                    }
                }

                POINT mousePos = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                if (uMsg == WM_NCMOUSEMOVE) ScreenToClient(static_cast<HWND>(window->GetHandle()), &mousePos);

                if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                {
                    MouseMovedEvent event{static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)};
                    event.Window = window.get();
                    data->EventCallback(event);
                }

                return 0;
            }

        case WM_MOUSELEAVE:
        case WM_NCMOUSELEAVE:
            {
                if (data == nullptr) return 0;
                TRACKMOUSEEVENT tme_cancel = { sizeof(tme_cancel), TME_CANCEL, static_cast<HWND>(window->GetNativeWindow()), 0 };
                TrackMouseEvent(&tme_cancel);
                data->MouseTrackedArea = 2;

                if (uMsg == WM_NCMOUSELEAVE)
                {
                    if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                    {
                        MouseLeftEvent event{};
                        event.Window = window.get();
                        data->EventCallback(event);
                    }
                }

                return 0;
            }

        case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
            {
                MouseButton pressedButton = MouseButton::Unknown;
                if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK) pressedButton = MouseButton::Left;
                if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONDBLCLK) pressedButton = MouseButton::Right;
                if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONDBLCLK) pressedButton = MouseButton::Middle;
                if (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONDBLCLK) pressedButton = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? MouseButton::X1 : MouseButton::X2;

                if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                {
                    MouseButtonPressedEvent event{pressedButton};
                    event.Window = window.get();
                    data->EventCallback(event);
                }

                return 0;
            }

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
            {
                MouseButton releasedButton = MouseButton::Unknown;
                if (uMsg == WM_LBUTTONUP) releasedButton = MouseButton::Left;
                if (uMsg == WM_RBUTTONUP) releasedButton = MouseButton::Right;
                if (uMsg == WM_MBUTTONUP) releasedButton = MouseButton::Middle;
                if (uMsg == WM_XBUTTONUP) releasedButton = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? MouseButton::X1 : MouseButton::X2;

                if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                {
                    MouseButtonReleasedEvent event{releasedButton};
                    event.Window = window.get();
                    data->EventCallback(event);
                }

                return 0;
            }

        case WM_MOUSEWHEEL:
            {
                if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                {
                    MouseScrolledEvent event{0.0f, static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA};
                    event.Window = window.get();
                    data->EventCallback(event);
                }

                return 0;
            }

        case WM_MOUSEHWHEEL:
            {
                if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                {
                    MouseScrolledEvent event{static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA, 0.0f};
                    event.Window = window.get();
                    data->EventCallback(event);
                }

                return 0;
            }

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            {
                WORD keyFlags = HIWORD(lParam);

                if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                {
                    KeyPressedEvent event(GetKeycode(wParam), (keyFlags & KF_REPEAT) == KF_REPEAT ? LOWORD(lParam) : 0);
                    event.Window = window.get();
                    data->EventCallback(event);
                }

                return 0;
            }

        case WM_KEYUP:
        case WM_SYSKEYUP:
            {
                if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                {
                    KeyReleasedEvent event{GetKeycode(wParam)};
                    event.Window = window.get();
                    data->EventCallback(event);
                }

                return 0;
            }

        case WM_CHAR:
            {
                if (IsWindowUnicode(static_cast<HWND>(window->GetHandle())))
                {
                    std::wstring ws(1, static_cast<wchar_t>(wParam));
                    std::string s(ws.begin(), ws.end());
                    char characterDef = s[0];

                    if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                    {
                        KeyTypedEvent event{characterDef};
                        event.Window = window.get();
                        data->EventCallback(event);
                    }

                    return 0;
                } else
                {
                    wchar_t character = 0;
                    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCSTR>(&wParam), 1, &character, 1);
                    std::wstring ws(&character);
                    std::string s(ws.begin(), ws.end());
                    char characterDef = s[0];

                    if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                    {
                        KeyTypedEvent event{characterDef};
                        event.Window = window.get();
                        data->EventCallback(event);
                    }

                    return 0;
                }
            }

        case WM_SETCURSOR:
            {
                // This is required to restore cursor when transitioning from e.g resize borders to client area.
                if (LOWORD(lParam) == HTCLIENT)
                    return 1;
                return 0;
            }

        case WM_DISPLAYCHANGE:
            {
                if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                {
                    MonitorsChangedEvent event{};
                    event.Window = window.get();
                    data->EventCallback(event);
                }

                return 0;
            }

        case WM_SETTINGCHANGE:
            {
                if (wParam == SPI_SETWORKAREA)
                {
                    if (data != nullptr && data->EventCallback != nullptr && window != nullptr)
                    {
                        MonitorsChangedEvent event{};
                        event.Window = window.get();
                        data->EventCallback(event);
                    }
                }
                return 0;
            }

        case WM_NCHITTEST:
            {
                if (data->HitTestCallback == nullptr) return HTCLIENT;
                auto [x, y] = window->GetPos();
                HitTest result = data->HitTestCallback(LOWORD(lParam), HIWORD(lParam));
                switch (result)
                {
                case HitTest::Client:
                    {
                        return HTCLIENT;
                    }
                case HitTest::TitleBar:
                    {
                        return HTCAPTION;
                    }
                case HitTest::ResizeTopLeft: return HTTOPLEFT;
                case HitTest::ResizeTop: return HTTOP;
                case HitTest::ResizeTopRight: return HTTOPRIGHT;
                case HitTest::ResizeLeft: return HTLEFT;
                case HitTest::ResizeRight: return HTRIGHT;
                case HitTest::ResizeBottomLeft: return HTBOTTOMLEFT;
                case HitTest::ResizeBottom: return HTBOTTOM;
                default: return HTNOWHERE;
                }
            }

        case WM_SYSCOMMAND:
            {
                break;
            }

        default: break;
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    void WindowsWindow::RegisterWindowClass()
    {
        s_hInstance = GetModuleHandle(nullptr);

        LPCSTR CLASS_NAME = "Nebula Window";

        s_WindowClass.cbSize = sizeof(WNDCLASSEX);
        s_WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        s_WindowClass.lpfnWndProc = OnEvent;
        s_WindowClass.hInstance = s_hInstance;
        s_WindowClass.lpszClassName = CLASS_NAME;

        s_WindowClassAtom = RegisterClassEx(&s_WindowClass);

        s_ClassRegistered = true;
    }

    KeyboardKey WindowsWindow::GetKeycode(DWORD keycode)
    {
        switch (keycode)
        {
            case VK_BACK: return KeyboardKey::Backspace;
            case VK_TAB: return KeyboardKey::Tab;
            case VK_CLEAR: return KeyboardKey::Clear;
            case VK_RETURN: return KeyboardKey::Return;
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
            case '0': return KeyboardKey::Num0;
            case '1': return KeyboardKey::Num1;
            case '2': return KeyboardKey::Num2;
            case '3': return KeyboardKey::Num3;
            case '4': return KeyboardKey::Num4;
            case '5': return KeyboardKey::Num5;
            case '6': return KeyboardKey::Num6;
            case '7': return KeyboardKey::Num7;
            case '8': return KeyboardKey::Num8;
            case '9': return KeyboardKey::Num9;
            case 'A': return KeyboardKey::A;
            case 'B': return KeyboardKey::B;
            case 'C': return KeyboardKey::C;
            case 'D': return KeyboardKey::D;
            case 'E': return KeyboardKey::E;
            case 'F': return KeyboardKey::F;
            case 'G': return KeyboardKey::G;
            case 'H': return KeyboardKey::H;
            case 'I': return KeyboardKey::I;
            case 'J': return KeyboardKey::J;
            case 'K': return KeyboardKey::K;
            case 'L': return KeyboardKey::L;
            case 'M': return KeyboardKey::M;
            case 'N': return KeyboardKey::N;
            case 'O': return KeyboardKey::O;
            case 'P': return KeyboardKey::P;
            case 'Q': return KeyboardKey::Q;
            case 'R': return KeyboardKey::R;
            case 'S': return KeyboardKey::S;
            case 'T': return KeyboardKey::T;
            case 'U': return KeyboardKey::U;
            case 'V': return KeyboardKey::V;
            case 'W': return KeyboardKey::W;
            case 'X': return KeyboardKey::X;
            case 'Y': return KeyboardKey::Y;
            case 'Z': return KeyboardKey::Z;
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
            case VK_SEPARATOR: return KeyboardKey::KeypadComma;
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
            default: break;
        }
        return KeyboardKey::Unknown;
    }

    LRESULT WindowsWindow::ErrorCallback(HWND hwnd, int nErrID, LPTSTR lpErrorText)
    {
        if (!hwnd) return FALSE;
        if (nErrID == 0) return TRUE;

        NB_CORE_ERROR("Win32 API Error: {}", lpErrorText);

        return TRUE;
    }
}
