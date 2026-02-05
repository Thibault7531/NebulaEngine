#include "LinuxWindow.h"

#include <xdg-shell-client-protocol.h>

#include "Platform/Nebula/CoreSystems/Core/Log.h"
#include "Platform/Platform/Linux/Rendering/OpenGLContext.h"

namespace Nebula::Platform
{
    void handle_global(void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t version)
    {
        if (strcmp(interface, wl_compositor_interface.name) == 0)
        {
            auto* compositor = static_cast<wl_compositor*>(wl_registry_bind(registry, name, &wl_compositor_interface, 3));
            static_cast<LinuxWindow*>(data)->SetWaylandCompositor(compositor);
        } else if (strcmp(interface, wl_shm_interface.name) == 0)
        {
            auto* shm = static_cast<wl_shm*>(wl_registry_bind(registry, name, &wl_shm_interface, 1));
            static_cast<LinuxWindow*>(data)->SetWaylandShm(shm);
        } else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
        {
            auto* wm_base = static_cast<xdg_wm_base*>(wl_registry_bind(registry, name, &xdg_wm_base_interface, 7));
            static_cast<LinuxWindow*>(data)->SetWaylandWMBase(wm_base);
        } else if (strcmp(interface, wl_seat_interface.name) == 0)
        {
            auto* seat = static_cast<wl_seat*>(wl_registry_bind(registry, name, &wl_seat_interface, 7));
            static_cast<LinuxWindow*>(data)->SetWaylandSeat(seat);
        }
        NB_PLATFORM_DEBUG("{}", interface);
    }

    void handle_global_remove(void* data, wl_registry* registry, uint32_t name)
    {
    }

    LinuxWindow::LinuxWindow(const WindowProperties& props)
    {
        m_Display = wl_display_connect(nullptr);

        m_Registry = wl_display_get_registry(m_Display);
        wl_registry_add_listener(m_Registry, &RegistryListener, this);
        wl_display_roundtrip(m_Display);

        m_Surface = wl_compositor_create_surface(m_Compositor);
        m_XDGSurface = xdg_wm_base_get_xdg_surface(m_WMBase, m_Surface);
        m_XDGToplevel = xdg_surface_get_toplevel(m_XDGSurface);

        xdg_toplevel_set_title(m_XDGToplevel, props.Title.c_str());
        xdg_toplevel_set_app_id(m_XDGToplevel, props.Title.c_str());
        if (props.Maximized) xdg_toplevel_set_maximized(m_XDGToplevel);
        if (props.Minimized) xdg_toplevel_set_minimized(m_XDGToplevel);

        wl_pointer* pointer = wl_seat_get_pointer(m_Seat);
        wl_pointer_add_listener(pointer, &PointerListener, this);

        wl_surface_commit(m_Surface);

        m_Context = CreateRef<OpenGlContext>(this);
    }

    LinuxWindow::~LinuxWindow() = default;

    void LinuxWindow::SetWindowHitTest(const std::function<HitTest(int32_t, int32_t)>& func) {}
    void LinuxWindow::RecalculateHitTest() {}
    const std::function<HitTest(int32_t, int32_t)>& LinuxWindow::GetWindowHitTest() {}

    void LinuxWindow::OnUpdate()
    {
        SwapBuffers();

        wl_display_dispatch(m_Display);
    }

    void LinuxWindow::SwapBuffers()
    {
        m_Context->SwapBuffers();
    }

    std::pair<uint32_t, uint32_t> LinuxWindow::GetSize() const
    {
        return {m_Properties.Width, m_Properties.Height};
    }

    void LinuxWindow::SetSize(uint32_t width, uint32_t height) {}
    std::pair<uint32_t, uint32_t> LinuxWindow::GetClientSize() const {}
    void LinuxWindow::SetClientSize(uint32_t width, uint32_t height) {}
    std::pair<int32_t, int32_t> LinuxWindow::GetPos() const {}
    void LinuxWindow::SetPos(int32_t x, int32_t y) {}
    std::pair<int32_t, int32_t> LinuxWindow::GetClientPos() const {}
    int32_t LinuxWindow::GetTitlebarHeight() const {}
    void LinuxWindow::SetMousePos(int32_t x, int32_t y) const {}
    void LinuxWindow::CaptureMouse(bool enabled) {}
    void LinuxWindow::SetVSync(VSyncMode mode) {}
    VSyncMode LinuxWindow::IsVSync() const {}
    void LinuxWindow::SetFullscreen(bool enabled) {}
    bool LinuxWindow::IsFullscreen() const {}
    void LinuxWindow::SetMaximized(bool enabled) {}
    bool LinuxWindow::IsMaximized() const {}
    void LinuxWindow::Minimize() {}
    bool LinuxWindow::IsMinimized() const {}
    bool LinuxWindow::IsFocused() const {}
    std::string LinuxWindow::GetTitle() const {}
    void LinuxWindow::SetTitle(const std::string& title) {}
    void LinuxWindow::Show() const {}
    void LinuxWindow::Hide() const {}
    void LinuxWindow::Raise() const {}
    void LinuxWindow::Flash() const {}
    Ref<Context> LinuxWindow::GetContext() {}
    void LinuxWindow::SetCurrentContext() {}
}
