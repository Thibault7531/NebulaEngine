#pragma once

#include <imgui.h>

#include "Nebula/CoreSystems/Core/Core.h"
#include "Nebula/CoreSystems/Core/Timestep.h"
#include "Nebula/Graphics/Buffers/Framebuffer.h"
#include "Platform/Nebula/CoreSystems/Core/PlatformUtils.h"
#include "Platform/Nebula/CoreSystems/Events/KeyEvent.h"
#include "Platform/Nebula/CoreSystems/Events/MouseEvent.h"
#include "Platform/Nebula/CoreSystems/Events/WindowEvents.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"

namespace Nebula
{

    struct PlatformData
    {
        Ref<Platform::Window> Window{};

        bool WantUpdateMonitors = true;

        Ref<Platform::Cursor> MouseCursors[ImGuiMouseCursor_COUNT];
        Ref<Platform::Cursor> MouseLastCursor{};
        int MousePendingLeaveFrame;
        int MouseButtonsDown;
        uint32_t MouseWindowID;

        std::string ClipboardTextData;
    };

    struct ViewportData
    {
        Ref<Platform::Window> Window;
        Ref<Platform::Window> ParentWindow;
        bool WindowOwned;
        Ref<Graphics::Surface> GraphicsSurface;
        Ref<Graphics::Framebuffer> Framebuffer;

        ViewportData() { memset((void*)this, 0, sizeof(*this)); }
    };

    class ImGuiPlatform
    {
    public:
        ImGuiPlatform()
            : io(ImGui::GetIO()), platform_io(ImGui::GetPlatformIO()) {}
        ~ImGuiPlatform() = default;

        void Init(const Ref<Platform::Window>& window);
        void Shutdown() const;

        void NewFrame(Timestep ts);

        void OnEvent(Platform::Event& event);

    private:
        void initMultipleViewports() const;
        void ShutdownMultipleViewports() const;

        bool OnMouseMovedEvent(const Platform::MouseMovedEvent& event) const;
        bool OnMouseScrolledEvent(const Platform::MouseScrolledEvent& event) const;
        bool OnMouseButtonPressedEvent(const Platform::MouseButtonPressedEvent& event);
        bool OnMouseButtonReleasedEvent(const Platform::MouseButtonReleasedEvent& event);
        bool OnMouseEnteredEvent(const Platform::MouseEnteredEvent& event);
        bool OnMouseLeftEvent(const Platform::MouseLeftEvent& event);
        bool OnKeyTypedEvent(const Platform::KeyTypedEvent& event) const;
        bool OnKeyPressedEvent(const Platform::KeyPressedEvent& event) const;
        bool OnKeyReleasedEvent(const Platform::KeyReleasedEvent& event) const;
        bool OnMonitorsChangedEvent(const Platform::MonitorsChangedEvent& event);
        bool OnWindowFocusedEvent(const Platform::WindowFocusedEvent& event) const;
        bool OnWindowLostFocusEvent(const Platform::WindowLostFocusEvent& event) const;
        bool OnWindowClosedEvent(const Platform::WindowClosedEvent& event) const;
        bool OnWindowMovedEvent(const Platform::WindowMovedEvent& event) const;
        bool OnWindowResizedEvent(const Platform::WindowResizedEvent& event) const;

        void UpdateMonitors();

        void UpdateMouseData() const;
        void UpdateMouseCursor();

        static void SetClipboardText(ImGuiContext*, const char* text);
        static const char* GetClipboardText(ImGuiContext*);

        static Ref<Platform::Window> GetWindowFromViewportID(ImGuiID viewport_id);

        static ImGuiKey getImGuiKey(KeyboardKey key);

    private:
        static void CreateImGuiWindow(ImGuiViewport* viewport);
        static void DestroyImGuiWindow(ImGuiViewport* viewport);
        static void ShowWindow(ImGuiViewport* viewport);
        static void UpdateWindow(ImGuiViewport* viewport);
        static ImVec2 GetWindowPos(ImGuiViewport* viewport);
        static void SetWindowPos(ImGuiViewport* viewport, ImVec2 pos);
        static ImVec2 GetWindowSize(ImGuiViewport* viewport);
        static bool GetWindowFocus(ImGuiViewport* viewport);
        static void SetWindowFocus(ImGuiViewport* viewport);
        static bool GetWindowMinimized(ImGuiViewport* viewport);
        static void SetWindowSize(ImGuiViewport* viewport, ImVec2 size);
        static void SetWindowTitle(ImGuiViewport* viewport, const char* title);
        static void RenderWindow(ImGuiViewport* viewport, void*);
        static void SwapBuffers(ImGuiViewport* viewport, void*);
        static void SetWindowAlpha(ImGuiViewport* viewport, float alpha);

    private:
        ImGuiIO& io;
        ImGuiPlatformIO& platform_io;

        PlatformData platform_data;

    public:
        static std::function<void(Platform::Event&)> s_EventCallback;
    };

}
