#include "iOSWindow.h"

namespace Nebula::Platform
{
    iOSWindow::iOSWindow(WindowProperties props)
        : m_NativeWindow{SwiftiOSWindow::CreateiOSWindow(props)} {}
    iOSWindow::~iOSWindow() {}
    void iOSWindow::SetEventCallback(const std::function<void(Event&)>& func) {}
    const std::function<void(Event&)>& iOSWindow::GetEventCallback() {}
    void iOSWindow::SetWindowHitTest(const std::function<HitTest(int32_t, int32_t)>& func) {}
    void iOSWindow::RecalculateHitTest() {}
    const std::function<HitTest(int32_t, int32_t)>& iOSWindow::GetWindowHitTest() {}
    void iOSWindow::OnUpdate() {}
    void iOSWindow::SwapBuffers() {}
    std::pair<uint32_t, uint32_t> iOSWindow::GetSize() const {}
    void iOSWindow::SetSize(uint32_t width, uint32_t height) {}
    std::pair<uint32_t, uint32_t> iOSWindow::GetClientSize() const {}
    void iOSWindow::SetClientSize(uint32_t width, uint32_t height) {}
    std::pair<int32_t, int32_t> iOSWindow::GetPos() const {}
    void iOSWindow::SetPos(int32_t x, int32_t y) {}
    std::pair<int32_t, int32_t> iOSWindow::GetClientPos() const {}
    int32_t iOSWindow::GetTitlebarHeight() const {}
    void iOSWindow::SetMousePos(int32_t x, int32_t y) const {}
    void iOSWindow::CaptureMouse(bool enabled) {}
    void iOSWindow::SetVSync(VSyncMode mode) {}
    VSyncMode iOSWindow::IsVSync() const {}
    void iOSWindow::SetFullscreen(bool enabled) {}
    bool iOSWindow::IsFullscreen() const {}
    void iOSWindow::SetMaximized(bool enabled) {}
    bool iOSWindow::IsMaximized() const {}
    void iOSWindow::Minimize() {}
    bool iOSWindow::IsMinimized() const {}
    bool iOSWindow::IsFocused() const {}
    std::string iOSWindow::GetTitle() const {}
    void iOSWindow::SetTitle(const std::string& title) {}
    void iOSWindow::Show() const {}
    void iOSWindow::Hide() const {}
    void iOSWindow::Raise() const {}
    void iOSWindow::Flash() const {}
    void iOSWindow::StartInput() {}
    void iOSWindow::StopInput() {}
    Ref<Context> iOSWindow::GetContext() {}
    void iOSWindow::SetCurrentContext() {}
    void* iOSWindow::GetNativeWindow() {}
    void* iOSWindow::GetHandle() {}
    uint32_t iOSWindow::GetID() const {}
}
