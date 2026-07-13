#include "MacOSWindow.h"

namespace Nebula::Platform
{
    MacOSWindow::MacOSWindow(WindowProperties props)
        : m_EventHandler{this}, m_NativeWindow{SwiftMacOSWindow::CreateMacOSWindow(props, m_EventHandler)}
    {
        m_Properties = props;
        if (m_Properties.API == GraphicsAPI::Metal3)
        {
            auto* mtlContext = static_cast<Graphics::Surface*>(m_NativeWindow.GetMetal3Context());
            m_GrahicsSurface = std::shared_ptr<Graphics::Surface>(mtlContext);
        }
        if (m_Properties.API == GraphicsAPI::OpenGL410)
        {
            auto* glContext = static_cast<Graphics::Surface*>(m_NativeWindow.GetOpenGLContext());
            m_GrahicsSurface = std::shared_ptr<Graphics::Surface>(glContext);
        }
    }

    MacOSWindow::~MacOSWindow()
    {
        m_NativeWindow.Destroy();
    }

    void MacOSWindow::SetEventCallback(const std::function<void(Event&)>& func) { m_EventCallback = func; }
    const std::function<void(Event&)>& MacOSWindow::GetEventCallback() { return m_EventCallback; }
    void MacOSWindow::SetWindowHitTest(const std::function<HitTest(int32_t, int32_t)>& func) { m_WindowHitTest = func; }
    void MacOSWindow::RecalculateHitTest() {}
    const std::function<HitTest(int32_t, int32_t)>& MacOSWindow::GetWindowHitTest() { return m_WindowHitTest; }

    void MacOSWindow::OnUpdate()
    {
        m_NativeWindow.OnUpdate();
    }

    void MacOSWindow::Present(void* framebuffer)
    {
        m_NativeWindow.Present(framebuffer);
    }

    std::pair<uint32_t, uint32_t> MacOSWindow::GetSize() const
    {
        return {m_NativeWindow.GetWindowWidth(), m_NativeWindow.GetWindowHeight()};
    }

    void MacOSWindow::SetSize(uint32_t width, uint32_t height)
    {
        m_NativeWindow.SetWindowSize(width, height);
    }

    std::pair<uint32_t, uint32_t> MacOSWindow::GetClientSize() const
    {
        return {m_NativeWindow.GetClientWidth(), m_NativeWindow.GetClientHeight()};
    }

    void MacOSWindow::SetClientSize(uint32_t width, uint32_t height)
    {
        m_NativeWindow.SetClientSize(width, height);
    }

    std::pair<int32_t, int32_t> MacOSWindow::GetPos() const
    {
        return {m_NativeWindow.GetX(), m_NativeWindow.GetY()};
    }

    void MacOSWindow::SetPos(int32_t x, int32_t y)
    {
        m_NativeWindow.SetPos(x, y);
    }

    std::pair<int32_t, int32_t> MacOSWindow::GetClientPos() const
    {
        return {m_NativeWindow.GetClientX(), m_NativeWindow.GetClientY()};
    }

    int32_t MacOSWindow::GetTitlebarHeight() const
    {
        return m_NativeWindow.GetTitlebarHeight();
    }

    void MacOSWindow::SetMousePos(int32_t x, int32_t y) const
    {
        m_NativeWindow.SetMousePos(x, y);
    }

    void MacOSWindow::CaptureMouse(bool enabled) {}

    void MacOSWindow::SetVSync(Graphics::VSyncMode mode)
    {
        m_NativeWindow.SetVSync(mode);
    }

    Graphics::VSyncMode MacOSWindow::IsVSync() const
    {
        return m_NativeWindow.IsVSync();
    }

    void MacOSWindow::SetFullscreen(bool enabled)
    {
        m_NativeWindow.SetFullscreen(enabled);
    }

    bool MacOSWindow::IsFullscreen() const
    {
        return m_NativeWindow.IsFullscreen();
    }
    
    void MacOSWindow::SetMaximized(bool enabled)
    {
        m_NativeWindow.SetMaximized(enabled);
    }

    bool MacOSWindow::IsMaximized() const
    {
        return m_NativeWindow.IsMaximized();
    }

    void MacOSWindow::Minimize()
    {
        m_NativeWindow.Minimize();
    }

    bool MacOSWindow::IsMinimized() const
    {
        return m_NativeWindow.IsMinimized();
    }

    bool MacOSWindow::IsFocused() const
    {
        return m_NativeWindow.IsFocused();
    }

    std::string MacOSWindow::GetTitle() const
    {
        return m_NativeWindow.GetTitle();
    }

    void MacOSWindow::SetTitle(const std::string& title)
    {
        m_NativeWindow.SetTitle(title);
    }

    void MacOSWindow::Show() const
    {
        m_NativeWindow.Show();
    }

    void MacOSWindow::Hide() const
    {
        m_NativeWindow.Hide();
    }

    void MacOSWindow::Raise() const
    {
        m_NativeWindow.Raise();
    }

    void MacOSWindow::Flash() const
    {
        m_NativeWindow.Flash();
    }

    Ref<Graphics::Surface> MacOSWindow::GetSurface()
    {
        return m_GrahicsSurface;
    }

    void* MacOSWindow::GetNativeWindow()
    {
        return &m_NativeWindow;
    }

    void* MacOSWindow::GetHandle()
    {
        return &m_NativeWindow;
    }

    uint32_t MacOSWindow::GetID() const
    {
        return m_NativeWindow.GetID();
    }
}
