#pragma once

#include <imgui.h>

#include "Nebula/CoreSystems/Core/Timestep.h"
#include "Nebula/CoreSystems/Windows/Window.h"
#include "Nebula/Events/ApplicationEvent.h"
#include "Nebula/Events/Event.h"
#include "Nebula/Events/KeyEvent.h"
#include "Nebula/Events/MouseEvent.h"
#include "Nebula/Utils/PlatformUtils.h"

namespace Nebula
{

    struct PlatformData
    {
        Ref<Window> Window{};

        bool WantUpdateMonitors = true;

        Ref<Cursor> MouseCursors[ImGuiMouseCursor_COUNT];
        Ref<Cursor> MouseLastCursor{};
        int MousePendingLeaveFrame;
        int MouseButtonsDown;
        uint32_t MouseWindowID;

        char* ClipboardTextData;

        PlatformData() { memset((void*)this, 0, sizeof(*this)); }
    };

    struct ViewportData
    {
        Ref<Window> Window;
        Ref<Nebula::Window> ParentWindow;
        bool WindowOwned;
        Ref<GraphicsContext> GraphicsContext;

        ViewportData() { memset((void*)this, 0, sizeof(*this)); }
    };

    class ImGuiPlatform
    {
    public:
        ImGuiPlatform()
            : io(ImGui::GetIO()), platform_io(ImGui::GetPlatformIO()) {}
        ~ImGuiPlatform() = default;

        void Init(const Ref<Window>& window);
        void Shutdown() const;

        void NewFrame(Timestep ts);

        void OnEvent(Event& event);

    private:
        void initMultipleViewports() const;
        void ShutdownMultipleViewports() const;

        bool OnMouseMovedEvent(const MouseMovedEvent& event) const;
        bool OnMouseScrolledEvent(const MouseScrolledEvent& event) const;
        bool OnMouseButtonPressedEvent(const MouseButtonPressedEvent& event);
        bool OnMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event);
        bool OnMouseEnteredEvent(const MouseEnteredEvent& event);
        bool OnMouseLeftEvent(const MouseLeftEvent& event);
        bool OnKeyTypedEvent(const KeyTypedEvent& event) const;
        bool OnKeyPressedEvent(const KeyPressedEvent& event) const;
        bool OnKeyReleasedEvent(const KeyReleasedEvent& event) const;
        bool OnMonitorsChangedEvent(const MonitorsChangedEvent& event);
        bool OnWindowFocusedEvent(const WindowFocusedEvent& event) const;
        bool OnWindowLostFocusEvent(const WindowLostFocusEvent& event) const;
        bool OnWindowClosedEvent(const WindowCloseEvent& event) const;
        bool OnWindowMovedEvent(const WindowMovedEvent& event) const;
        bool OnWindowResizedEvent(const WindowResizeEvent& event) const;

        void UpdateMonitors();

        void UpdateMouseData() const;
        void UpdateMouseCursor();

        static void SetClipboardText(ImGuiContext*, const char* text);
        static const char* GetClipboardText(ImGuiContext*);

        static Ref<Window> GetWindowFromViewportID(ImGuiID viewport_id);

        static ImGuiKey getImGuiKey(KeyboardKey key);

    private:
        static void CreateImGuiWindow(ImGuiViewport* viewport);
        static void DestroyImGuiWindow(ImGuiViewport* viewport);
        static void ShowWindow(ImGuiViewport* viewport);
        static void UpdateWIndow(ImGuiViewport* viewport);
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
        static std::function<void(Event&)> s_EventCallback;
    };

}