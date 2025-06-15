#pragma once

#include <imgui.h>
#include <SDL.h>

#include "Nebula/Events/ApplicationEvent.h"
#include "Nebula/Events/KeyEvent.h"
#include "Nebula/Events/MouseEvent.h"
#include "Nebula/ImGui/ImGuiPlatform.h"

namespace Nebula
{
    struct PlatformData
    {
        SDL_WindowID windowID;
        SDL_Window* ImeWindow;
        Uint64 Time;

        bool WantUpdateMonitors = true;

        SDL_Cursor* MouseCursors[ImGuiMouseCursor_COUNT];
        SDL_Cursor* MouseLastCursor;
        int MousePendingLeaveFrame;
        int MouseButtonsDown;
        uint32_t MouseWindowID;

        char* ClipboardTextData;

        PlatformData() { memset(this, 0, sizeof(*this)); }
    };

    struct ViewportData
    {
        SDL_Window* Window;
        SDL_Window* ParentWindow;
        uint32_t WindowID;
        bool WindowOwned;
        SDL_GLContext GLContext;

        ViewportData() { memset(this, 0, sizeof(*this)); }
    };

    class WindowsImGuiPlatform : public ImGuiPlatform
    {
    public:
        WindowsImGuiPlatform()
            : io(ImGui::GetIO()), platform_io(ImGui::GetPlatformIO()) {}
        virtual ~WindowsImGuiPlatform() {}

        virtual void Init(Window* window) override;
        virtual void Shutdown() override;

        virtual void NewFrame() override;

        virtual void OnEvent(Event& event) override;

        virtual void SetInputStatus() override;

        void initMultipleViewports();
        void ShutdownMultipleViewports();

    private:
        bool OnMouseMovedEvent(const MouseMovedEvent& event);
        bool OnMouseScrolledEvent(const MouseScrolledEvent& event);
        bool OnMouseButtonPressedEvent(const MouseButtonPressedEvent& event);
        bool OnMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event);
        bool OnMouseEnteredEvent(const MouseEnteredEvent& event);
        bool OnMouseLeftEvent(const MouseLeftEvent& event);
        bool OnKeyTypedEvent(const KeyTypedEvent& event);
        bool OnKeyPressedEvent(const KeyPressedEvent& event);
        bool OnKeyReleasedEvent(const KeyReleasedEvent& event);
        bool OnMonitorsChangedEvent(const MonitorsChangedEvent& event);
        bool OnWindowFocusedEvent(const WindowFocusedEvent& event);
        bool OnWindowLostFocusEvent(const WindowLostFocusEvent& event);
        bool OnWindowClosedEvent(const WindowCloseEvent& event);
        bool OnWindowMovedEvent(const WindowMovedEvent& event);
        bool OnWindowResizedEvent(const WindowResizeEvent& event);

        void UpdateMonitors();

        void UpdateMouseData();
        void UpdateMouseCursor();

        static void SetClipboardText(ImGuiContext*, const char* text);
        static const char* GetClipboardText(ImGuiContext*);
        static void SetIMEData(ImGuiContext*, ImGuiViewport* viewport, ImGuiPlatformImeData* IMEData);

        static SDL_Window* GetWindowFromViewportID(ImGuiID viewport_id);

        static ImGuiKey getImGuiKey(int keycode);

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

        SDL_Window* m_Window;

    public:
        static std::function<void(Event&)> s_EventCallback;
    };

}
