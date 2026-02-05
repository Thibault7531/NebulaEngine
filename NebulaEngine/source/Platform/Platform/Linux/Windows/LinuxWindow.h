#pragma once

#include <wayland-client.h>
#include <xdg-shell-client-protocol.h>

#include "Platform/Platform/Linux/Events/EventHandler.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"

namespace Nebula::Platform
{
    void handle_global(void* data, wl_registry* registry, uint32_t name, const char* interface, uint32_t version);
    void handle_global_remove(void* data, wl_registry* registry, uint32_t name);

    inline wl_registry_listener RegistryListener {
        .global = handle_global,
        .global_remove = handle_global_remove
    };

    inline wl_pointer_listener PointerListener {
        .enter = EventHandler::OnMouseEnterEvent,
        .leave = EventHandler::OnMouseLeaveEvent,
        .motion = EventHandler::OnMouseMoveEvent,
        .button = EventHandler::OnButtonEvent,
        .axis = EventHandler::OnMouseScrolledEvent,
        .frame = EventHandler::OnMouseFrameEvent,
        .axis_source = EventHandler::OnMouseAxisSourceEvent,
        .axis_stop = EventHandler::OnMouseAxisStopEvent,
        .axis_discrete = EventHandler::OnMouseAxisDiscreteEvent
    };

    class LinuxWindow : public Window
    {
    public:
        LinuxWindow(const WindowProperties& props);
        virtual ~LinuxWindow();

        virtual void SetEventCallback(const std::function<void(Event&)>& func) override { m_EventCallback = func; }
        virtual const std::function<void(Event&)>& GetEventCallback() override {return m_EventCallback; }
        virtual void SetWindowHitTest(const std::function<HitTest(int32_t, int32_t)>& func) override;
        virtual void RecalculateHitTest() override;
        virtual const std::function<HitTest(int32_t, int32_t)>& GetWindowHitTest() override;

        virtual void OnUpdate() override;
        virtual void SwapBuffers() override;

        [[nodiscard]] virtual std::pair<uint32_t, uint32_t> GetSize() const override;
        virtual void SetSize(uint32_t width, uint32_t height) override;
        [[nodiscard]] virtual std::pair<uint32_t, uint32_t> GetClientSize() const override;
        virtual void SetClientSize(uint32_t width, uint32_t height) override;
        [[nodiscard]] virtual std::pair<int32_t, int32_t> GetPos() const override;
        virtual void SetPos(int32_t x, int32_t y) override;
        [[nodiscard]] virtual std::pair<int32_t, int32_t> GetClientPos() const override;
        [[nodiscard]] virtual int32_t GetTitlebarHeight() const override;

        virtual void SetMousePos(int32_t x, int32_t y) const override;
        virtual void CaptureMouse(bool enabled) override;

        virtual void SetVSync(VSyncMode mode) override;
        [[nodiscard]] virtual VSyncMode IsVSync() const override;
        virtual void SetFullscreen(bool enabled) override;
        [[nodiscard]] virtual bool IsFullscreen() const override;
        virtual void SetMaximized(bool enabled) override;
        [[nodiscard]] virtual bool IsMaximized() const override;
        virtual void Minimize() override;
        [[nodiscard]] virtual bool IsMinimized() const override;
        [[nodiscard]] virtual bool IsFocused() const override;
        [[nodiscard]] virtual std::string GetTitle() const override;
        virtual void SetTitle(const std::string& title) override;

        virtual void Show() const override;
        virtual void Hide() const override;
        virtual void Raise() const override;
        virtual void Flash() const override;

        virtual Ref<Context> GetContext() override;
        virtual void SetCurrentContext() override;

    public:
        virtual void* GetNativeWindow() override { return m_Surface; }
        [[nodiscard]] virtual void* GetHandle() const override { return m_Surface; }
        [[nodiscard]] virtual uint32_t GetID() const override { return 0; }

    public:
        [[nodiscard]] wl_display* GetWaylandDisplay() const { return m_Display; }
        void SetWaylandCompositor(wl_compositor* compositor) { m_Compositor = compositor; }
        void SetWaylandWMBase(xdg_wm_base* wm_base) { m_WMBase = wm_base; }
        void SetWaylandSeat(wl_seat* seat) { m_Seat = seat; }
        void SetWaylandShm(wl_shm* shm) { m_Shm = shm; }

    private:
        wl_display* m_Display;
        wl_registry* m_Registry;
        wl_compositor* m_Compositor;
        wl_seat* m_Seat;
        xdg_wm_base* m_WMBase;
        wl_surface* m_Surface;
        xdg_surface* m_XDGSurface;
        wl_shm* m_Shm;
        wl_shm_pool* m_ShmPool;
        wl_buffer* m_Buffer;
        xdg_toplevel* m_XDGToplevel;

        Ref<Context> m_Context;

        std::function<void(Event&)> m_EventCallback;
    };

}
