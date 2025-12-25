#include "WindowsWindow.h"

#include <Windows.h>
#include <windows.ui.interop.h>
#include <winrt/Microsoft.UI.Dispatching.h>

#include "../../../../../vendor/imgui/imgui.h"
#include "Platform/Nebula/CoreSystems/Core/CoreUtils.h"
#include "Platform/Platform/Windows/Rendering/OpenGLContext.h"

namespace Nebula::Platform
{
    winrt::Microsoft::UI::Dispatching::DispatcherQueueController WindowsWindow::s_DispatcherQueueController{nullptr};
    bool WindowsWindow::s_DispatcherQueueControllerInitialized = false;

    WindowsWindow::WindowsWindow(const WindowProperties& props)
        : m_Window{nullptr}
    {
        m_Properties = props;

        winrt::Microsoft::UI::Windowing::AppWindowPresenter presenter = nullptr;

        switch (m_Properties.Style)
        {
        case WindowStyle::Default:
        case WindowStyle::BorderlessInteractive:
        case WindowStyle::BorderlessFullscreen:
            {
                winrt::Microsoft::UI::Windowing::OverlappedPresenter overlappedPresenter = winrt::Microsoft::UI::Windowing::OverlappedPresenter::Create();

                overlappedPresenter.IsAlwaysOnTop(m_Properties.AlwaysOnTop);
                overlappedPresenter.IsMaximizable(m_Properties.isResizable);
                overlappedPresenter.IsMinimizable(m_Properties.isResizable);
                overlappedPresenter.IsResizable(m_Properties.isResizable);

                if (m_Properties.Maximized) overlappedPresenter.Maximize();

                overlappedPresenter.SetBorderAndTitleBar(m_Properties.Style != WindowStyle::BorderlessFullscreen, m_Properties.Style != WindowStyle::BorderlessFullscreen);

                presenter = overlappedPresenter;

                break;
            }

        case WindowStyle::ExclusiveFullscreen:
            {
                presenter = winrt::Microsoft::UI::Windowing::FullScreenPresenter::Create();

                break;
            }
        }

        m_Window = winrt::Microsoft::UI::Windowing::AppWindow::Create(presenter);

        m_Window.Title(winrt::to_hstring(m_Properties.Title));
        m_Window.TitleBar().PreferredTheme(winrt::Microsoft::UI::Windowing::TitleBarTheme::UseDefaultAppMode);
        if (m_Properties.x != -INT_MAX && m_Properties.y != -INT_MAX) m_Window.Move({m_Properties.x, m_Properties.y});

        winrt::Windows::Graphics::SizeInt32 size;
        size.Width = m_Properties.Width;
        size.Height = m_Properties.Height;
        m_Window.ResizeClient(size);

        switch (m_Properties.Style)
        {
        case WindowStyle::BorderlessInteractive:
            {
                m_Window.TitleBar().ExtendsContentIntoTitleBar(true);
                m_Window.TitleBar().ButtonBackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
                m_Window.TitleBar().ButtonHoverBackgroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(50, 0, 0, 0));

                break;
            }

        default: break;
        }

        m_Window.Show(true);
        GetWindowFromWindowId({m_Window.Id().Value}, &m_Hwnd);

        if (!s_DispatcherQueueControllerInitialized)
        {
            s_DispatcherQueueController = winrt::Microsoft::UI::Dispatching::DispatcherQueueController::CreateOnCurrentThread();
            s_DispatcherQueueControllerInitialized = true;
        }
        m_Window.AssociateWithDispatcherQueue(s_DispatcherQueueController.DispatcherQueue());
        //m_DispatcherQueueController.DispatcherQueue().EnqueueEventLoopExit();
        //m_DispatcherQueueController.DispatcherQueue().RunEventLoop();

        m_WindowEventHandler = CreateRef<WindowEventHandler>(this);
        m_WindowEventHandler->InitEventHandlers();
        m_MouseEventHandler = CreateRef<MouseEventHandler>(this);
        m_MouseEventHandler->InitEventHandlers();
        m_KeyboardEventHandler = CreateRef<KeyboardEventHandler>(this);
        m_KeyboardEventHandler->InitEventHandlers();

        m_Context = CreateRef<OpenGlContext>(this);
        m_Context->Init();
    }

    WindowsWindow::~WindowsWindow()
    {
        //m_DispatcherQueueController.ShutdownQueue();
        //s_DispatcherQueueController.DispatcherQueue().EnqueueEventLoopExit();
        //s_DispatcherQueueController.DispatcherQueue().RunEventLoop();
        m_Context.reset();
        m_Window.Destroy();
        m_KeyboardEventHandler.reset();
        m_MouseEventHandler.reset();
        m_WindowEventHandler.reset();
    }

    void WindowsWindow::SetEventCallback(const std::function<void(Event&)>& callback)
    {
        m_WindowEventHandler->SetEventCallback(callback);
        m_MouseEventHandler->SetEventCallback(callback);
        m_KeyboardEventHandler->SetEventCallback(callback);
    }

    void WindowsWindow::SetWindowHitTest(const std::function<HitTest(int32_t, int32_t)>& func)
    {
        // NOT SUPPORTED BY WINDOWS APP SDK (yet?)
    }

    void WindowsWindow::OnUpdate()
    {
        m_Context->SwapBuffers();

        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        //m_DispatcherQueueController.DispatcherQueue().EnqueueEventLoopExit();
        //m_DispatcherQueueController.DispatcherQueue().RunEventLoop();
    }

    void WindowsWindow::SwapBuffers()
    {
        m_Context->SwapBuffers();
    }

    std::pair<unsigned int, unsigned int> WindowsWindow::GetSize() const
    {
        auto size = m_Window.Size();
        return {size.Width, size.Height};
    }

    void WindowsWindow::SetSize(uint32_t width, uint32_t height)
    {
        m_Window.ResizeClient({static_cast<int32_t>(width), static_cast<int32_t>(height)});
    }

    std::pair<uint32_t, uint32_t> WindowsWindow::GetPixelSize() const
    {
        auto size = m_Window.ClientSize();
        return {size.Width, size.Height};
    }

    std::pair<int32_t, int32_t> WindowsWindow::GetPos() const
    {
        auto pos = m_Window.Position();
        return {pos.X, pos.Y};
    }

    void WindowsWindow::SetPos(int32_t x, int32_t y)
    {
        m_Window.Move({x, y});
    }

    void WindowsWindow::SetMousePos(int32_t x, int32_t y) const
    {
        auto WindowPos = m_Window.Position();
        HWND window;
        GetWindowFromWindowId({m_Window.Id().Value}, &window);
        ClientToScreen(window, reinterpret_cast<LPPOINT>(&WindowPos));
        SetCursorPos(WindowPos.X + x, WindowPos.Y + y);
    }

    void WindowsWindow::CaptureMouse(bool enabled)
    {
        SetCapture(enabled ? m_Hwnd : nullptr);
    }

    void WindowsWindow::SetVSync(VSyncMode mode)
    {
        m_Context->SetVSync(mode);
    }

    VSyncMode WindowsWindow::IsVSync() const
    {
        return m_Context->IsVsync();
    }

    void WindowsWindow::SetFullscreen(bool enabled)
    {
        if (enabled && m_Properties.Style == WindowStyle::ExclusiveFullscreen) return;
        if (!enabled && m_Properties.Style != WindowStyle::ExclusiveFullscreen) return;

        if (enabled)
        {
            m_LastStyle = m_Properties.Style;
            m_Properties.Style = WindowStyle::ExclusiveFullscreen;

            winrt::Microsoft::UI::Windowing::FullScreenPresenter presenter = winrt::Microsoft::UI::Windowing::FullScreenPresenter::Create();
            m_Window.SetPresenter(presenter);
        }
        else
        {
            m_Properties.Style = m_LastStyle;

            winrt::Microsoft::UI::Windowing::OverlappedPresenter overlappedPresenter = winrt::Microsoft::UI::Windowing::OverlappedPresenter::Create();

            overlappedPresenter.IsAlwaysOnTop(m_Properties.AlwaysOnTop);
            overlappedPresenter.IsMaximizable(m_Properties.isResizable);
            overlappedPresenter.IsMinimizable(m_Properties.isResizable);
            overlappedPresenter.IsResizable(m_Properties.isResizable);

            if (m_Properties.Maximized) overlappedPresenter.Maximize();

            overlappedPresenter.SetBorderAndTitleBar(m_Properties.Style != WindowStyle::BorderlessFullscreen, m_Properties.Style != WindowStyle::BorderlessFullscreen);

            m_Window.SetPresenter(overlappedPresenter);
        }
    }

    bool WindowsWindow::IsFullscreen() const
    {
        return m_Properties.Style == WindowStyle::ExclusiveFullscreen;
    }

    void WindowsWindow::SetMaximized(bool enabled)
    {
        if (m_Properties.Style == WindowStyle::BorderlessFullscreen || m_Properties.Style == WindowStyle::ExclusiveFullscreen) return;

        m_Properties.Maximized = enabled;

        winrt::Microsoft::UI::Windowing::OverlappedPresenter presenter = m_Window.Presenter().as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>();
        if (enabled) presenter.Maximize();
        else presenter.Restore();
    }

    bool WindowsWindow::IsMaximized() const
    {
        if (m_Properties.Style == WindowStyle::BorderlessFullscreen || m_Properties.Style == WindowStyle::ExclusiveFullscreen) return true;

        return m_Window.Presenter().as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>().State() == winrt::Microsoft::UI::Windowing::OverlappedPresenterState::Maximized;
    }

    void WindowsWindow::Minimize()
    {
        if (m_Properties.Style == WindowStyle::BorderlessFullscreen || m_Properties.Style == WindowStyle::ExclusiveFullscreen) return;

        winrt::Microsoft::UI::Windowing::OverlappedPresenter presenter = m_Window.Presenter().as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>();
        presenter.Minimize();
    }

    bool WindowsWindow::IsMinimized() const
    {
        if (m_Properties.Style == WindowStyle::ExclusiveFullscreen) return false;

        return m_Window.Presenter().as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>().State() == winrt::Microsoft::UI::Windowing::OverlappedPresenterState::Minimized;
    }

    bool WindowsWindow::IsFocused() const
    {
        return GetFocus() == m_Hwnd;
    }

    std::string WindowsWindow::GetTitle() const
    {
        return m_Properties.Title;
    }

    void WindowsWindow::SetTitle(const std::string& title)
    {
        m_Properties.Title = title;
        m_Window.Title(winrt::to_hstring(title));
    }

    void WindowsWindow::Show() const
    {
        m_Window.Show();
    }

    void WindowsWindow::Hide() const
    {
        m_Window.Hide();
    }

    void WindowsWindow::Raise() const
    {
        m_Window.MoveInZOrderAtTop();
    }

    void WindowsWindow::Flash() const
    {
        FLASHWINFO flashInfo = {
            .cbSize = sizeof(FLASHWINFO),
            .hwnd = m_Hwnd,
            .dwFlags = FLASHW_TRAY | FLASHW_TIMERNOFG,
            .uCount = 0,
            .dwTimeout = 0
        };
        FlashWindowEx(&flashInfo);
    }

    void WindowsWindow::SetCurrentContext()
    {
        m_Context->SetCurrent();
    }

    void* WindowsWindow::GetHandle() const
    {
        return m_Hwnd;
    }

    uint32_t WindowsWindow::GetID() const
    {
        return m_Window.Id().Value;
    }
}
