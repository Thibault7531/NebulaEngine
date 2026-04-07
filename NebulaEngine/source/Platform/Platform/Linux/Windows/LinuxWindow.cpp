#include "LinuxWindow.h"

#include "Platform/Nebula/CoreSystems/Core/CoreUtils.h"
#include "Platform/Nebula/CoreSystems/Core/Log.h"
#include "Platform/Platform/Linux/Rendering/OpenGLContext.h"

namespace Nebula::Platform
{

    LinuxWindow::LinuxWindow(const WindowProperties& props)
    {
        m_Properties = props;

        SDL_WindowFlags windowFlags = 0;
        if (m_Properties.Minimized) windowFlags |= SDL_WINDOW_MINIMIZED;
        if (m_Properties.Maximized) windowFlags |= SDL_WINDOW_MAXIMIZED;
        if (m_Properties.AlwaysOnTop) windowFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
        if (m_Properties.isUtility) windowFlags |= SDL_WINDOW_UTILITY;

        if (m_Properties.Style == WindowStyle::ExclusiveFullscreen)
        {
            windowFlags |= SDL_WINDOW_FULLSCREEN;
        } else if (m_Properties.Style == WindowStyle::BorderlessFullscreen)
        {
            windowFlags |= SDL_WINDOW_BORDERLESS;
        } else if (m_Properties.Style == WindowStyle::BorderlessInteractive)
        {
            windowFlags |= SDL_WINDOW_BORDERLESS;
            if (m_Properties.isResizable) windowFlags |= SDL_WINDOW_RESIZABLE;
        } else if (m_Properties.Style == WindowStyle::Default)
        {
            if (m_Properties.isResizable) windowFlags |= SDL_WINDOW_RESIZABLE;
        }

        windowFlags |= SDL_WINDOW_OPENGL;

        m_Window = SDL_CreateWindow(m_Properties.Title.c_str(), m_Properties.Width, m_Properties.Height, windowFlags);
        SDL_SetWindowPosition(m_Window, m_Properties.x, m_Properties.y);

        SDL_SetEventFilter(EventHandler::OnEvent, nullptr);
        SDL_SetWindowHitTest(m_Window, EventHandler::OnHitTest, this);

        m_Context = CreateRef<OpenGlContext>(this);
        m_Context->Init();
    }

    LinuxWindow::~LinuxWindow()
    {
        m_Context.reset();
        SDL_DestroyWindow(m_Window);
    }

    void LinuxWindow::SetWindowHitTest(const std::function<HitTest(int32_t, int32_t)>& func)
    {
        m_WindowHitTest = func;
        RecalculateHitTest();
    }

    void LinuxWindow::OnUpdate()
    {
        SwapBuffers();

        CoreUtils::PumpEvents();
    }

    void LinuxWindow::SwapBuffers()
    {
        m_Context->SwapBuffers();
    }

    std::pair<uint32_t, uint32_t> LinuxWindow::GetSize() const
    {
        int width, height;
        SDL_GetWindowSize(m_Window, &width, &height);
        return {width, height};
    }

    void LinuxWindow::SetSize(uint32_t width, uint32_t height)
    {
        SDL_SetWindowSize(m_Window, width, height);
        SDL_SyncWindow(m_Window);

        int newWidth, newWHight;
        SDL_GetWindowSizeInPixels(m_Window, &newWidth, &newWHight);
        m_Properties.Width = newWidth;
        m_Properties.Height = newWHight;
    }

    std::pair<uint32_t, uint32_t> LinuxWindow::GetClientSize() const
    {
        int width, height;
        SDL_GetWindowSizeInPixels(m_Window, &width, &height);
        return {width, height};
    }

    void LinuxWindow::SetClientSize(uint32_t width, uint32_t height)
    {
        m_Properties.Width = width;
        m_Properties.Height = height;

        SDL_SetWindowSize(m_Window, width, height);
    }

    std::pair<int32_t, int32_t> LinuxWindow::GetPos() const
    {
        int x, y;
        SDL_GetWindowPosition(m_Window, &x, &y);
        return {x, y};
    }

    void LinuxWindow::SetPos(int32_t x, int32_t y)
    {
        m_Properties.x = x;
        m_Properties.y = y;
        SDL_SetWindowPosition(m_Window, x, y);
    }

    std::pair<int32_t, int32_t> LinuxWindow::GetClientPos() const
    {
        int x, y;
        SDL_GetWindowPosition(m_Window, &x, &y);
        return {x, y};
    }

    int32_t LinuxWindow::GetTitlebarHeight() const
    {
        return 32;
    }

    void LinuxWindow::SetMousePos(int32_t x, int32_t y) const
    {
        SDL_WarpMouseInWindow(m_Window, x, y);
    }

    void LinuxWindow::CaptureMouse(bool enabled)
    {
        SDL_CaptureMouse(enabled);
    }

    void LinuxWindow::SetVSync(VSyncMode mode)
    {
        m_Context->SetVSync(mode);
    }

    VSyncMode LinuxWindow::IsVSync() const
    {
        return m_Context->IsVsync();
    }

    void LinuxWindow::SetFullscreen(bool enabled)
    {
        if (enabled && m_Properties.Style == WindowStyle::ExclusiveFullscreen) return;
        if (!enabled && m_Properties.Style != WindowStyle::ExclusiveFullscreen) return;

        if (enabled)
        {
            m_LastStyle = m_Properties.Style;
            m_Properties.Style = WindowStyle::ExclusiveFullscreen;
        }
        else
        {
            m_Properties.Style = m_LastStyle;
        }
        SDL_SetWindowFullscreen(m_Window, enabled);
    }

    bool LinuxWindow::IsFullscreen() const
    {
        return m_Properties.Style == WindowStyle::ExclusiveFullscreen;
    }

    void LinuxWindow::SetMaximized(bool enabled)
    {
        if (m_Properties.Style == WindowStyle::BorderlessFullscreen || m_Properties.Style == WindowStyle::ExclusiveFullscreen) return;
        m_Properties.Maximized = enabled;
        if (enabled)
        {
            SDL_MaximizeWindow(m_Window);
        }
        else
        {
            SDL_RestoreWindow(m_Window);
        }
    }

    bool LinuxWindow::IsMaximized() const
    {
        return m_Properties.Maximized;
    }

    void LinuxWindow::Minimize()
    {
        m_Properties.Minimized = true;
        SDL_MinimizeWindow(m_Window);
    }

    bool LinuxWindow::IsMinimized() const
    {
        return m_Properties.Minimized;
    }

    bool LinuxWindow::IsFocused() const
    {
        return SDL_GetWindowFlags(m_Window) & SDL_WINDOW_INPUT_FOCUS;
    }

    std::string LinuxWindow::GetTitle() const
    {
        return SDL_GetWindowTitle(m_Window);
    }

    void LinuxWindow::SetTitle(const std::string& title)
    {
        SDL_SetWindowTitle(m_Window, title.c_str());
    }

    void LinuxWindow::Show() const
    {
        SDL_ShowWindow(m_Window);
    }

    void LinuxWindow::Hide() const
    {
        SDL_HideWindow(m_Window);
    }

    void LinuxWindow::Raise() const
    {
        SDL_RaiseWindow(m_Window);
    }

    void LinuxWindow::Flash() const
    {
        SDL_FlashOperation operation = {SDL_FLASH_UNTIL_FOCUSED};
        SDL_FlashWindow(m_Window, operation);
    }

    void LinuxWindow::StartInput()
    {
        SDL_StartTextInput(m_Window);
    }

    void LinuxWindow::StopInput()
    {
        SDL_StopTextInput(m_Window);
    }

    Ref<Context> LinuxWindow::GetContext()
    {
        return m_Context;
    }

    void LinuxWindow::SetCurrentContext()
    {
        m_Context->SetCurrent();
    }
}
