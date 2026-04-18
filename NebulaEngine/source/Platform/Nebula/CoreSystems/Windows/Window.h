#pragma once

#include <string>
#include <thread>
#include <Core/Nebula/CoreSystems/Core/Core.h>

#include "Platform/Nebula/CoreSystems/Events/Event.h"
#include "Platform/Nebula/CoreSystems/Rendering/Context.h"

namespace Nebula::Platform
{

    enum class WindowStyle
    {
        Default,
        BorderlessInteractive,
        BorderlessFullscreen,
        ExclusiveFullscreen
    };

    enum class HitTest
    {
        None,
        Client,
        TitleBar,
        ResizeTopLeft,
        ResizeTop,
        ResizeTopRight,
        ResizeRight,
        ResizeBottomRight,
        ResizeBottom,
        ResizeBottomLeft,
        ResizeLeft
    };

    struct WindowProperties
    {
        std::string Title = "Nebula Engine";

        uint32_t Width = 1920;
        uint32_t Height = 1080;

        int32_t x = -INT_MAX;
        int32_t y = -INT_MAX;

        WindowStyle Style = WindowStyle::Default;

        bool Maximized = false;
        bool Minimized = false;

        bool isResizable = true;
        bool AlwaysOnTop = false;
        bool isUtility = false;

        // Don't change
        bool isMouseHovering = false;
        bool isCursorChanged = false;
    };

    class Window
    {
    public:
        virtual ~Window() = default;

        virtual void SetEventCallback(const std::function<void(Event &)>& func) = 0;
        virtual const std::function<void(Event&)>& GetEventCallback() = 0;
        virtual void SetWindowHitTest(const std::function<HitTest(int32_t, int32_t)>& func) = 0;
        virtual void RecalculateHitTest() = 0;
        virtual const std::function<HitTest(int32_t, int32_t)>& GetWindowHitTest() = 0;

        virtual void OnUpdate() = 0;
        virtual void SwapBuffers() = 0;

        [[nodiscard]] virtual std::pair<uint32_t, uint32_t> GetSize() const = 0;
        virtual void SetSize(uint32_t width, uint32_t height) = 0;
        [[nodiscard]] virtual std::pair<uint32_t, uint32_t> GetClientSize() const = 0;
        virtual void SetClientSize(uint32_t width, uint32_t height) = 0;
        [[nodiscard]] virtual std::pair<int32_t, int32_t> GetPos() const = 0;
        virtual void SetPos(int32_t x, int32_t y) = 0;
        [[nodiscard]] virtual std::pair<int32_t, int32_t> GetClientPos() const = 0;
        [[nodiscard]] virtual int32_t GetTitlebarHeight() const = 0;

        virtual void SetMousePos(int32_t x, int32_t y) const = 0;
        virtual void CaptureMouse(bool enabled) = 0;

        virtual void SetVSync(VSyncMode mode) = 0;
        [[nodiscard]] virtual VSyncMode IsVSync() const = 0;
        virtual void SetFullscreen(bool enabled) = 0;
        [[nodiscard]] virtual bool IsFullscreen() const = 0;
        virtual void SetMaximized(bool enabled) = 0;
        [[nodiscard]] virtual bool IsMaximized() const = 0;
        virtual void Minimize() = 0;
        [[nodiscard]] virtual bool IsMinimized() const = 0;
        [[nodiscard]] virtual bool IsFocused() const = 0;
        [[nodiscard]] virtual std::string GetTitle() const = 0;
        virtual void SetTitle(const std::string& title) = 0;

        virtual void Show() const = 0;
        virtual void Hide() const = 0;
        virtual void Raise() const = 0;
        virtual void Flash() const = 0;

        virtual void StartInput() = 0;
        virtual void StopInput() = 0;

        [[nodiscard]] virtual Ref<Context> GetContext() = 0;
        virtual void SetCurrentContext() = 0;

    public:
        WindowProperties& GetProperties() { return m_Properties; }

    public:
        virtual void* GetNativeWindow() = 0;
        virtual void* GetHandle() = 0;
        virtual uint32_t GetID() const = 0;

    public:
        static Ref<Window> Create(const WindowProperties& props);

    protected:
        WindowProperties m_Properties;

        WindowStyle m_LastStyle{};
    };

}
