#include "WindowsWindow.h"

#include <CommCtrl.h>
#include <Windows.h>
#include <windows.ui.interop.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Windows.Foundation.Collections.h>

#include "Platform/Platform/Windows/Rendering/OpenGLContext.h"

namespace Nebula::Platform
{

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

        GetWindowFromWindowId({m_Window.Id().Value}, &m_Hwnd);

        switch (m_Properties.Style)
        {
        case WindowStyle::BorderlessInteractive:
            {
                m_Window.TitleBar().ExtendsContentIntoTitleBar(true);
                m_Window.TitleBar().ButtonBackgroundColor(winrt::Microsoft::UI::Colors::Transparent());
                m_Window.TitleBar().ButtonHoverBackgroundColor(winrt::Microsoft::UI::ColorHelper::FromArgb(50, 0, 0, 0));

                winrt::Windows::Graphics::SizeInt32 size;
                size.Width = m_Properties.Width;
                size.Height = m_Properties.Height;
                m_Window.ResizeClient(size);

                break;
            }

        case WindowStyle::BorderlessFullscreen:
            {
                winrt::Windows::Graphics::SizeInt32 size;
                size.Width = m_Properties.Width;
                size.Height = m_Properties.Height;
                m_Window.Resize(size);
                break;
            }

        default:
            {
                winrt::Windows::Graphics::SizeInt32 size;
                size.Width = m_Properties.Width;
                size.Height = m_Properties.Height;
                m_Window.ResizeClient(size);
                break;
            }
        }

        SetWindowSubclass(m_Hwnd, EventHandler::SubclassProc, 0, reinterpret_cast<DWORD_PTR>(this));

        m_Window.Show(true);

        m_Context = CreateRef<OpenGlContext>(this);
        m_Context->Init();
    }

    WindowsWindow::~WindowsWindow()
    {
        m_Context.reset();
        m_Window.Destroy();
    }

    void WindowsWindow::SetEventCallback(const std::function<void(Event&)>& callback)
    {
        m_EventCallback = callback;
    }

    const std::function<void(Event&)>& WindowsWindow::GetEventCallback()
    {
        return m_EventCallback;
    }

    void WindowsWindow::SetWindowHitTest(const std::function<HitTest(int32_t, int32_t)>& func)
    {
        m_WindowHitTest = func;
        RecalculateHitTest();
    }

    void WindowsWindow::RecalculateHitTest()
    {
        std::vector<winrt::Windows::Graphics::RectInt32> dragRects = FindDragRectsFromHitTest();

        m_Window.TitleBar().SetDragRectangles(dragRects);
    }

    const std::function<HitTest(int32_t, int32_t)>& WindowsWindow::GetWindowHitTest()
    {
        return m_WindowHitTest;
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
        m_Window.Resize({static_cast<int32_t>(width), static_cast<int32_t>(height)});
    }

    std::pair<uint32_t, uint32_t> WindowsWindow::GetClientSize() const
    {
        auto size = m_Window.ClientSize();
        return {size.Width, size.Height};
    }

    void WindowsWindow::SetClientSize(uint32_t width, uint32_t height)
    {
        if (GetProperties().Style == WindowStyle::BorderlessFullscreen) m_Window.Resize({static_cast<int32_t>(width), static_cast<int32_t>(height)});
        else m_Window.ResizeClient({static_cast<int32_t>(width), static_cast<int32_t>(height)});
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

    std::pair<int32_t, int32_t> WindowsWindow::GetClientPos() const
    {
        RECT rect;
        GetClientRect(m_Hwnd, &rect);
        //ClientToScreen(m_Hwnd, reinterpret_cast<LPPOINT>(&rect));
        MapWindowPoints(m_Hwnd, nullptr, reinterpret_cast<LPPOINT>(&rect), 2);
        return {rect.left, rect.top};
    }

    int32_t WindowsWindow::GetTitlebarHeight() const
    {
        return m_Window.TitleBar().Height();
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

    std::vector<winrt::Windows::Graphics::RectInt32> WindowsWindow::FindDragRectsFromHitTest()
    {
        if (!m_WindowHitTest) return {};

        struct span
        {
            int32_t x_start, x_end;
            bool merged;
        };
        std::vector<winrt::Windows::Graphics::RectInt32> finalRects;
        std::vector<winrt::Windows::Graphics::RectInt32> activeRects;
        int32_t width = GetClientSize().first;
        int32_t height = GetTitlebarHeight();

        for (int32_t y = 0; y < height; y++)
        {
            std::vector<span> rowSpans;

            for (int32_t x = 0; x < width; x++)
            {
                if (m_WindowHitTest(x, y) == HitTest::TitleBar)
                {
                    int32_t x_start = x;
                    while (x < width && m_WindowHitTest(x, y) == HitTest::TitleBar) x++;
                    rowSpans.push_back({x_start, x-1, false});
                }
            }

            std::vector<winrt::Windows::Graphics::RectInt32> nextActiveRects;

            for (auto& rect : activeRects)
            {
                bool foundMatch = false;
                for (auto& span : rowSpans)
                {
                    if (!span.merged && span.x_start == rect.X && (span.x_end - span.x_start + 1) == rect.Width)
                    {
                        rect.Height++;
                        span.merged = true;
                        nextActiveRects.push_back(rect);
                        foundMatch = true;
                        break;
                    }
                }

                if (!foundMatch) finalRects.push_back(rect);
            }

            for (const auto& span : rowSpans)
            {
                if (!span.merged)
                {
                    nextActiveRects.push_back({
                        span.x_start,
                        y,
                        span.x_end - span.x_start + 1,
                        1
                    });
                }
            }

            activeRects = nextActiveRects;
        }

        finalRects.insert(finalRects.end(), activeRects.begin(), activeRects.end());

        return finalRects;
    }
}
