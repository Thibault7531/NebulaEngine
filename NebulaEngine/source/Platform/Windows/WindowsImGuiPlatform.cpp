#include "WindowsImGuiPlatform.h"

#include <imgui_internal.h>

#include "WindowsWindow.h"
#include "Nebula/Core/Input.h"
#include "Nebula/Core/KeyCodes.h"
#include "Nebula/Events/ApplicationEvent.h"

namespace Nebula
{
    std::function<void(Event&)> WindowsImGuiPlatform::s_EventCallback = nullptr;

    void WindowsImGuiPlatform::Init(Window* window)
    {
        NB_CORE_DEBUG("Initializing Nebula::Platform::Windows for ImGui");

        // Bind Event Callback
        s_EventCallback = NB_BIND_EVENT_FN(WindowsImGuiPlatform::OnEvent);

        // Store window
        m_Window = static_cast<SDL_Window*>(window->GetNativeWindow());

        // Set backend data
        platform_data.windowID = SDL_GetWindowID(m_Window);
        io.BackendPlatformUserData = static_cast<void*>(&platform_data);

        // Set backend flags
        io.BackendPlatformName = "Nebula::Platform::Windows";
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;

        // Sets backendio
        platform_io.Platform_SetClipboardTextFn = SetClipboardText;
        platform_io.Platform_GetClipboardTextFn = GetClipboardText;
        platform_io.Platform_SetImeDataFn = SetIMEData;

        // UpdateMonitors()
        UpdateMonitors();

        // Load mouse cursors
        platform_data.MouseCursors[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
        platform_data.MouseCursors[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
        platform_data.MouseCursors[ImGuiMouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
        platform_data.MouseCursors[ImGuiMouseCursor_NotAllowed] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);

        // SetupPlatformHandles
        ImGuiViewport* mainViewport = ImGui::GetMainViewport();
        mainViewport->PlatformHandle = reinterpret_cast<void*>(static_cast<intptr_t>(SDL_GetWindowID(m_Window)));
        mainViewport->PlatformHandleRaw = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(m_Window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));

        // Set SDL Hints
        SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
        SDL_SetHint(SDL_HINT_MOUSE_AUTO_CAPTURE, "0");
        SDL_SetHint("SDL_BORDERLESS_WINDOWED_STYLE", "0");

        // initMultipleViewports
        initMultipleViewports();
    }

    void WindowsImGuiPlatform::Shutdown()
    {
        NB_CORE_DEBUG("Shutting down {0}", io.BackendPlatformName);

        ShutdownMultipleViewports();

        if (platform_data.ClipboardTextData) SDL_free(platform_data.ClipboardTextData);
        for (const auto & MouseCursor : platform_data.MouseCursors) SDL_DestroyCursor(MouseCursor);

        io.BackendPlatformName = nullptr;
        io.BackendPlatformUserData = nullptr;
        io.BackendFlags &= ~(ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_HasGamepad | ImGuiBackendFlags_PlatformHasViewports | ImGuiBackendFlags_HasMouseHoveredViewport);
    }

    void WindowsImGuiPlatform::NewFrame()
    {
        // Setup display size (every frame to accommodate for window resizing)
        int w, h;
        int display_w, display_h;
        SDL_GetWindowSize(m_Window, &w, &h);
        if (SDL_GetWindowFlags(m_Window) & SDL_WINDOW_MINIMIZED) w = h = 0;
        SDL_GetWindowSizeInPixels(m_Window, &display_w, &display_h);
        io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
        if (w > 0 && h > 0) io.DisplayFramebufferScale = ImVec2(static_cast<float>(display_w) / w, static_cast<float>(display_h) / h);

        // Update monitors
        if (platform_data.WantUpdateMonitors) UpdateMonitors();

        // Setup time step (we don't use SDL_GetTicks() because it is using millisecond resolution)
        // (Accept SDL_GetPerformanceCounter() not returning a monotonically increasing value. Happens in VMs and Emscripten, see #6189, #6114, #3644)
        static Uint64 frequency = SDL_GetPerformanceFrequency();
        Uint64 current_time = SDL_GetPerformanceCounter();
        if (current_time <= platform_data.Time)
            current_time = platform_data.Time + 1;
        io.DeltaTime = platform_data.Time > 0 ? static_cast<float>(static_cast<double>(current_time - platform_data.Time) / frequency) : (float)(1.0f / 60.0f);
        platform_data.Time = current_time;

        if (platform_data.MousePendingLeaveFrame && platform_data.MousePendingLeaveFrame >= ImGui::GetFrameCount() && platform_data.MouseButtonsDown == 0)
        {
            platform_data.MouseWindowID = 0;
            platform_data.MousePendingLeaveFrame = 0;
            io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
        }


        // Our io.AddMouseViewportEvent() calls will only be valid when not capturing.
        // Technically speaking testing for 'bd->MouseButtonsDown == 0' would be more rigorous, but testing for payload reduces noise and potential side-effects.
        if (ImGui::GetDragDropPayload() == nullptr)
            io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;
        else
            io.BackendFlags &= ~ImGuiBackendFlags_HasMouseHoveredViewport;

        UpdateMouseData();
        UpdateMouseCursor();
    }

    void WindowsImGuiPlatform::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<MouseMovedEvent>(NB_BIND_EVENT_FN(OnMouseMovedEvent));
        dispatcher.Dispatch<MouseScrolledEvent>(NB_BIND_EVENT_FN(OnMouseScrolledEvent));
        dispatcher.Dispatch<MouseButtonPressedEvent>(NB_BIND_EVENT_FN(OnMouseButtonPressedEvent));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(NB_BIND_EVENT_FN(OnMouseButtonReleasedEvent));
        dispatcher.Dispatch<MouseEnteredEvent>(NB_BIND_EVENT_FN(OnMouseEnteredEvent));
        dispatcher.Dispatch<MouseLeftEvent>(NB_BIND_EVENT_FN(OnMouseLeftEvent));
        dispatcher.Dispatch<KeyTypedEvent>(NB_BIND_EVENT_FN(OnKeyTypedEvent));
        dispatcher.Dispatch<KeyPressedEvent>(NB_BIND_EVENT_FN(OnKeyPressedEvent));
        dispatcher.Dispatch<KeyReleasedEvent>(NB_BIND_EVENT_FN(OnKeyReleasedEvent));
        dispatcher.Dispatch<MonitorsChangedEvent>(NB_BIND_EVENT_FN(OnMonitorsChangedEvent));
        dispatcher.Dispatch<WindowFocusedEvent>(NB_BIND_EVENT_FN(OnWindowFocusedEvent));
        dispatcher.Dispatch<WindowLostFocusEvent>(NB_BIND_EVENT_FN(OnWindowLostFocusEvent));
        dispatcher.Dispatch<WindowCloseEvent>(NB_BIND_EVENT_FN(OnWindowClosedEvent));
        dispatcher.Dispatch<WindowMovedEvent>(NB_BIND_EVENT_FN(OnWindowMovedEvent));
        dispatcher.Dispatch<WindowResizeEvent>(NB_BIND_EVENT_FN(OnWindowResizedEvent));
    }

    void WindowsImGuiPlatform::SetInputStatus()
    {
        SDL_Window* window = SDL_GL_GetCurrentWindow();
        if (io.WantTextInput) {
            SDL_StartTextInput(window);
        } else {
            SDL_StopTextInput(window);
        }
    }

    bool WindowsImGuiPlatform::OnMouseMovedEvent(const MouseMovedEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(SDL_GetWindowID(static_cast<SDL_Window*>(event.Window->GetNativeWindow()))))) == nullptr) return false;

        ImVec2 mouse_pos(event.GetX(), event.GetY());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            int window_x, window_y;
            SDL_GetWindowPosition(static_cast<SDL_Window*>(event.Window->GetNativeWindow()), &window_x, &window_y);
            mouse_pos.x += window_x;
            mouse_pos.y += window_y;
        }
        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMousePosEvent(mouse_pos.x, mouse_pos.y);

        return false;
    }

    bool WindowsImGuiPlatform::OnMouseScrolledEvent(const MouseScrolledEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(SDL_GetWindowID(static_cast<SDL_Window*>(event.Window->GetNativeWindow()))))) == nullptr) return false;

        float wheel_x = event.GetXOffset();
        float wheel_y = event.GetYOffset();
        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMouseWheelEvent(wheel_x, wheel_y);
        return false;
    }

    bool WindowsImGuiPlatform::OnMouseButtonPressedEvent(const MouseButtonPressedEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(SDL_GetWindowID(static_cast<SDL_Window*>(event.Window->GetNativeWindow()))))) == nullptr) return false;

        int mouse_button = -1;
        if (event.GetMouseButton() == SDL_BUTTON_LEFT) { mouse_button = 0; }
        if (event.GetMouseButton() == SDL_BUTTON_RIGHT) { mouse_button = 1; }
        if (event.GetMouseButton() == SDL_BUTTON_MIDDLE) { mouse_button = 2; }
        if (event.GetMouseButton() == SDL_BUTTON_X1) { mouse_button = 3; }
        if (event.GetMouseButton() == SDL_BUTTON_X2) { mouse_button = 4; }
        if (mouse_button == -1)
            return false;

        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMouseButtonEvent(mouse_button, true);
        platform_data.MouseButtonsDown = platform_data.MouseButtonsDown | (1 << mouse_button);
        return false;
    }

    bool WindowsImGuiPlatform::OnMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(SDL_GetWindowID(static_cast<SDL_Window*>(event.Window->GetNativeWindow()))))) == nullptr) return false;

        int mouse_button = -1;
        if (event.GetMouseButton() == SDL_BUTTON_LEFT) { mouse_button = 0; }
        if (event.GetMouseButton() == SDL_BUTTON_RIGHT) { mouse_button = 1; }
        if (event.GetMouseButton() == SDL_BUTTON_MIDDLE) { mouse_button = 2; }
        if (event.GetMouseButton() == SDL_BUTTON_X1) { mouse_button = 3; }
        if (event.GetMouseButton() == SDL_BUTTON_X2) { mouse_button = 4; }
        if (mouse_button == -1)
            return false;

        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMouseButtonEvent(mouse_button, false);
        platform_data.MouseButtonsDown = platform_data.MouseButtonsDown & ~(1 << mouse_button);
        return false;
    }

    bool WindowsImGuiPlatform::OnKeyTypedEvent(const KeyTypedEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(SDL_GetWindowID(static_cast<SDL_Window*>(event.Window->GetNativeWindow()))))) == nullptr) return false;

        int keycode = event.GetKeyCode();
        if (keycode > 0 && keycode < 0x10000)
            io.AddInputCharacter(keycode);
        return true;
    }

    bool WindowsImGuiPlatform::OnKeyPressedEvent(const KeyPressedEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(SDL_GetWindowID(static_cast<SDL_Window*>(event.Window->GetNativeWindow()))))) == nullptr) return false;

        io.AddKeyEvent(ImGuiMod_Ctrl, Input::IsKeyPressed(NB_KEY_LCTRL) || Input::IsKeyPressed(NB_KEY_RCTRL));
        io.AddKeyEvent(ImGuiMod_Shift, Input::IsKeyPressed(NB_KEY_LSHIFT) || Input::IsKeyPressed(NB_KEY_RSHIFT));
        io.AddKeyEvent(ImGuiMod_Alt, Input::IsKeyPressed(NB_KEY_LALT) || Input::IsKeyPressed(NB_KEY_RALT));
        io.AddKeyEvent(ImGuiMod_Super, Input::IsKeyPressed(NB_KEY_LGUI) || Input::IsKeyPressed(NB_KEY_RGUI));

        ImGuiKey key = getImGuiKey(event.GetKeyCode());
        io.AddKeyEvent(key, true);
        io.SetKeyEventNativeData(key, event.GetKeyCode(), SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(event.GetKeyCode()), nullptr));

        return false;
    }

    bool WindowsImGuiPlatform::OnKeyReleasedEvent(const KeyReleasedEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(SDL_GetWindowID(static_cast<SDL_Window*>(event.Window->GetNativeWindow()))))) == nullptr) return false;

        io.AddKeyEvent(ImGuiMod_Ctrl, Input::IsKeyPressed(NB_KEY_LCTRL) || Input::IsKeyPressed(NB_KEY_RCTRL));
        io.AddKeyEvent(ImGuiMod_Shift, Input::IsKeyPressed(NB_KEY_LSHIFT) || Input::IsKeyPressed(NB_KEY_RSHIFT));
        io.AddKeyEvent(ImGuiMod_Alt, Input::IsKeyPressed(NB_KEY_LALT) || Input::IsKeyPressed(NB_KEY_RALT));
        io.AddKeyEvent(ImGuiMod_Super, Input::IsKeyPressed(NB_KEY_LGUI) || Input::IsKeyPressed(NB_KEY_RGUI));

        ImGuiKey key = getImGuiKey(event.GetKeyCode());
        io.AddKeyEvent(key, false);
        io.SetKeyEventNativeData(key, event.GetKeyCode(), SDL_GetScancodeFromKey(static_cast<SDL_Keycode>(event.GetKeyCode()), nullptr));

        return false;
    }

    bool WindowsImGuiPlatform::OnMonitorsChangedEvent(const MonitorsChangedEvent& event)
    {
        platform_data.WantUpdateMonitors = true;

        return false;
    }

    bool WindowsImGuiPlatform::OnWindowFocusedEvent(const WindowFocusedEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(SDL_GetWindowID(static_cast<SDL_Window*>(event.Window->GetNativeWindow()))))) == nullptr) return false;

        io.AddFocusEvent(true);
        return false;
    }

    bool WindowsImGuiPlatform::OnWindowLostFocusEvent(const WindowLostFocusEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(SDL_GetWindowID(static_cast<SDL_Window*>(event.Window->GetNativeWindow()))))) == nullptr) return false;

        io.AddFocusEvent(false);
        return false;
    }

    bool WindowsImGuiPlatform::OnWindowClosedEvent(const WindowCloseEvent& event)
    {
        ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(reinterpret_cast<intptr_t>(static_cast<SDL_Window*>(event.Window->GetNativeWindow()))));
        if (viewport == nullptr)
            return false;

        viewport->PlatformRequestClose = true;

        return true;
    }

    bool WindowsImGuiPlatform::OnWindowMovedEvent(const WindowMovedEvent& event)
    {
        ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(reinterpret_cast<intptr_t>(static_cast<SDL_Window*>(event.Window->GetNativeWindow()))));
        if (viewport == nullptr)
            return false;

        viewport->PlatformRequestMove = true;

        return true;
    }

    bool WindowsImGuiPlatform::OnWindowResizedEvent(const WindowResizeEvent& event)
    {
        ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(reinterpret_cast<intptr_t>(static_cast<SDL_Window*>(event.Window->GetNativeWindow()))));
        if (viewport == nullptr)
            return false;

        viewport->PlatformRequestResize = true;

        return true;
    }

    bool WindowsImGuiPlatform::OnMouseEnteredEvent(const MouseEnteredEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(SDL_GetWindowID(static_cast<SDL_Window*>(event.Window->GetNativeWindow()))))) == nullptr) return false;

        platform_data.MouseWindowID = SDL_GetWindowID(static_cast<SDL_Window*>(event.Window->GetNativeWindow()));
        platform_data.MousePendingLeaveFrame = 0;

        return false;
    }

    bool WindowsImGuiPlatform::OnMouseLeftEvent(const MouseLeftEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(SDL_GetWindowID(static_cast<SDL_Window*>(event.Window->GetNativeWindow()))))) == nullptr) return false;

        platform_data.MousePendingLeaveFrame = ImGui::GetFrameCount();

        return false;
    }

    void WindowsImGuiPlatform::UpdateMonitors()
    {
        platform_io.Monitors.resize(0);
        platform_data.WantUpdateMonitors = false;

        int display_count;
        SDL_DisplayID* displays = SDL_GetDisplays(&display_count);
        for (int n = 0; n < display_count; n++)
        {
            SDL_DisplayID display_id = displays[n];
            ImGuiPlatformMonitor monitor;
            SDL_Rect r;

            SDL_GetDisplayBounds(display_id, &r);
            monitor.MainPos = monitor.WorkPos = ImVec2(static_cast<float>(r.x), static_cast<float>(r.y));
            monitor.MainSize = monitor.WorkSize = ImVec2(static_cast<float>(r.w), static_cast<float>(r.h));

            SDL_GetDisplayUsableBounds(display_id, &r);
            monitor.WorkPos = ImVec2(static_cast<float>(r.x), static_cast<float>(r.y));
            monitor.WorkSize = ImVec2(static_cast<float>(r.w), static_cast<float>(r.h));

            monitor.DpiScale = SDL_GetDisplayContentScale(display_id);
            monitor.PlatformHandle = reinterpret_cast<void*>(static_cast<intptr_t>(n));
            if (monitor.DpiScale <= 0.0f)
                continue; // Some accessibility applications are declaring virtual monitors with a DPI of 0, see #7902.
            platform_io.Monitors.push_back(monitor);
        }
        SDL_free(displays);
    }

    void WindowsImGuiPlatform::initMultipleViewports()
    {
        // Register platform interface (will be coupled with a renderer interface)
        platform_io.Platform_CreateWindow = CreateImGuiWindow;
        platform_io.Platform_DestroyWindow = DestroyImGuiWindow;
        platform_io.Platform_ShowWindow = ShowWindow;
        platform_io.Platform_UpdateWindow = UpdateWIndow;
        platform_io.Platform_SetWindowPos = SetWindowPos;
        platform_io.Platform_GetWindowPos = GetWindowPos;
        platform_io.Platform_SetWindowSize = SetWindowSize;
        platform_io.Platform_GetWindowSize = GetWindowSize;
        platform_io.Platform_SetWindowFocus = SetWindowFocus;
        platform_io.Platform_GetWindowFocus = GetWindowFocus;
        platform_io.Platform_GetWindowMinimized = GetWindowMinimized;
        platform_io.Platform_SetWindowTitle = SetWindowTitle;
        platform_io.Platform_RenderWindow = RenderWindow;
        platform_io.Platform_SwapBuffers = SwapBuffers;
        platform_io.Platform_SetWindowAlpha = SetWindowAlpha;


        // Register main window handle (which is owned by the main application, not by us)
        // This is mostly for simplicity and consistency, so that our code (e.g. mouse handling etc.) can use same logic for main and secondary viewports.
        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        auto* viewport_data = new ViewportData;
        viewport_data->Window = m_Window;
        viewport_data->WindowID = SDL_GetWindowID(m_Window);
        viewport_data->WindowOwned = false;
        viewport_data->GLContext = SDL_GL_GetCurrentContext();
        main_viewport->PlatformUserData = viewport_data;
        main_viewport->PlatformHandle = reinterpret_cast<void*>(static_cast<intptr_t>(viewport_data->WindowID));
    }

    void WindowsImGuiPlatform::ShutdownMultipleViewports()
    {
        ImGui::DestroyPlatformWindows();
    }

    void WindowsImGuiPlatform::UpdateMouseData()
    {
        // We forward mouse input when hovered or captured (via SDL_EVENT_MOUSE_MOTION) or when focused (below)
        // SDL_CaptureMouse() let the OS know e.g. that our imgui drag outside the SDL window boundaries shouldn't e.g. trigger other operations outside
        SDL_CaptureMouse(platform_data.MouseButtonsDown != 0);
        SDL_Window* focused_window = SDL_GetKeyboardFocus();

        const bool is_app_focused = (focused_window && (m_Window == focused_window || ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(SDL_GetWindowID(focused_window)))) != nullptr));


        if (is_app_focused)
        {
            // (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when io.ConfigNavMoveSetMousePos is enabled by user)
            if (io.WantSetMousePos)
            {
                if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                    SDL_WarpMouseGlobal(io.MousePos.x, io.MousePos.y);
                else
                    SDL_WarpMouseInWindow(m_Window, io.MousePos.x, io.MousePos.y);
            }

            // (Optional) Fallback to provide mouse position when focused (SDL_EVENT_MOUSE_MOTION already provides this when hovered or captured)
            if (platform_data.MouseButtonsDown == 0)
            {
                // Single-viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
                // Multi-viewport mode: mouse position in OS absolute coordinates (io.MousePos is (0,0) when the mouse is on the upper-left of the primary monitor)
                float mouse_x, mouse_y;
                int window_x, window_y;
                SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
                if (!(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
                {
                    SDL_GetWindowPosition(focused_window, &window_x, &window_y);
                    mouse_x -= window_x;
                    mouse_y -= window_y;
                }
                io.AddMousePosEvent((float)mouse_x, (float)mouse_y);
            }
        }

        // (Optional) When using multiple viewports: call io.AddMouseViewportEvent() with the viewport the OS mouse cursor is hovering.
        // If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the backend, Dear imGui will ignore this field and infer the information using its flawed heuristic.
        // - [!] SDL backend does NOT correctly ignore viewports with the _NoInputs flag.
        //       Some backend are not able to handle that correctly. If a backend report an hovered viewport that has the _NoInputs flag (e.g. when dragging a window
        //       for docking, the viewport has the _NoInputs flag in order to allow us to find the viewport under), then Dear ImGui is forced to ignore the value reported
        //       by the backend, and use its flawed heuristic to guess the viewport behind.
        // - [X] SDL backend correctly reports this regardless of another viewport behind focused and dragged from (we need this to find a useful drag and drop target).
        if (io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport)
        {
            ImGuiID mouse_viewport_id = 0;
            if (ImGuiViewport* mouse_viewport = ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(platform_data.MouseWindowID))))
                mouse_viewport_id = mouse_viewport->ID;
            io.AddMouseViewportEvent(mouse_viewport_id);
        }

    }

    void WindowsImGuiPlatform::UpdateMouseCursor()
    {
        if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
            return;

        ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
        if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
        {
            // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
            SDL_HideCursor();
        }
        else
        {
            // Show OS mouse cursor
            SDL_Cursor* expected_cursor = platform_data.MouseCursors[imgui_cursor] ? platform_data.MouseCursors[imgui_cursor] : platform_data.MouseCursors[ImGuiMouseCursor_Arrow];
            if (platform_data.MouseLastCursor != expected_cursor)
            {
                SDL_SetCursor(expected_cursor); // SDL function doesn't have an early out (see #6113)
                platform_data.MouseLastCursor = expected_cursor;
            }
            SDL_ShowCursor();
        }
    }

    void WindowsImGuiPlatform::SetClipboardText(ImGuiContext*, const char* text)
    {
        SDL_SetClipboardText(text);
    }

    const char* WindowsImGuiPlatform::GetClipboardText(ImGuiContext*)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto* backend_data = static_cast<PlatformData*>(io.BackendPlatformUserData);

        if (backend_data->ClipboardTextData) SDL_free(backend_data->ClipboardTextData);

        const char* sdl_clipboard_text = SDL_GetClipboardText();
        backend_data->ClipboardTextData = sdl_clipboard_text ? SDL_strdup(sdl_clipboard_text) : NULL;

        return backend_data->ClipboardTextData;
    }

    void WindowsImGuiPlatform::SetIMEData(ImGuiContext*, ImGuiViewport* viewport, ImGuiPlatformImeData* IMEData)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto* backend_data = static_cast<PlatformData*>(io.BackendPlatformUserData);
        SDL_Window* window = SDL_GetWindowFromID(backend_data->windowID);

        if ((IMEData->WantVisible == false || backend_data->ImeWindow != window) && backend_data->ImeWindow != NULL)
        {
            SDL_StopTextInput(backend_data->ImeWindow);
            backend_data->ImeWindow = nullptr;
        }

        if (IMEData->WantVisible)
        {
            SDL_Rect r;
            r.x = static_cast<int>(IMEData->InputPos.x - viewport->Pos.x);
            r.y = static_cast<int>(IMEData->InputPos.y - viewport->Pos.y + IMEData->InputLineHeight);
            r.w = 1;
            r.h = static_cast<int>(IMEData->InputLineHeight);
            SDL_SetTextInputArea(window, &r, 0);

            SDL_StartTextInput(window);
            backend_data->ImeWindow = window;
        }
    }

    SDL_Window* WindowsImGuiPlatform::GetWindowFromViewportID(ImGuiID viewport_id)
    {
        if (viewport_id != 0)
            if (ImGuiViewport* viewport = ImGui::FindViewportByID(viewport_id))
            {
                SDL_WindowID window_id = static_cast<SDL_WindowID>(reinterpret_cast<intptr_t>(viewport->PlatformHandle));
                return SDL_GetWindowFromID(window_id);
            }
        return nullptr;
    }

    ImGuiKey WindowsImGuiPlatform::getImGuiKey(int keycode)
    {
        switch (keycode)
        {
        case NB_KEY_TAB: return ImGuiKey_Tab;
        case NB_KEY_LEFT: return ImGuiKey_LeftArrow;
        case NB_KEY_RIGHT: return ImGuiKey_RightArrow;
        case NB_KEY_UP: return ImGuiKey_UpArrow;
        case NB_KEY_DOWN: return ImGuiKey_DownArrow;
        case NB_KEY_PAGEUP: return ImGuiKey_PageUp;
        case NB_KEY_PAGEDOWN: return ImGuiKey_PageDown;
        case NB_KEY_HOME: return ImGuiKey_Home;
        case NB_KEY_END: return ImGuiKey_End;
        case NB_KEY_INSERT: return ImGuiKey_Insert;
        case NB_KEY_DELETE: return ImGuiKey_Delete;
        case NB_KEY_BACKSPACE: return ImGuiKey_Backspace;
        case NB_KEY_SPACE: return ImGuiKey_Space;
        case NB_KEY_RETURN: return ImGuiKey_Enter;
        case NB_KEY_ESCAPE: return ImGuiKey_Escape;
        case NB_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
        case NB_KEY_COMMA: return ImGuiKey_Comma;
        case NB_KEY_MINUS: return ImGuiKey_Minus;
        case NB_KEY_PERIOD: return ImGuiKey_Period;
        case NB_KEY_SLASH: return ImGuiKey_Slash;
        case NB_KEY_SEMICOLON: return ImGuiKey_Semicolon;
        case NB_KEY_EQUALS: return ImGuiKey_Equal;
        case NB_KEY_LEFTBRACKET: return ImGuiKey_LeftBracket;
        case NB_KEY_BACKSLASH: return ImGuiKey_Backslash;
        case NB_KEY_RIGHTBRACKET: return ImGuiKey_RightBracket;
        case NB_KEY_GRAVE: return ImGuiKey_GraveAccent;
        case NB_KEY_CAPSLOCK: return ImGuiKey_CapsLock;
        case NB_KEY_SCROLLLOCK: return ImGuiKey_ScrollLock;
        case NB_KEY_NUMLOCKCLEAR: return ImGuiKey_NumLock;
        case NB_KEY_PRINTSCREEN: return ImGuiKey_PrintScreen;
        case NB_KEY_PAUSE: return ImGuiKey_Pause;
        case NB_KEY_LCTRL: return ImGuiKey_LeftCtrl;
        case NB_KEY_LSHIFT: return ImGuiKey_LeftShift;
        case NB_KEY_LALT: return ImGuiKey_LeftAlt;
        case NB_KEY_LGUI: return ImGuiKey_LeftSuper;
        case NB_KEY_RCTRL: return ImGuiKey_RightCtrl;
        case NB_KEY_RSHIFT: return ImGuiKey_RightShift;
        case NB_KEY_RALT: return ImGuiKey_RightAlt;
        case NB_KEY_RGUI: return ImGuiKey_RightSuper;
        case NB_KEY_APPLICATION: return ImGuiKey_Menu;
        case NB_KEY_0: return ImGuiKey_0;
        case NB_KEY_1: return ImGuiKey_1;
        case NB_KEY_2: return ImGuiKey_2;
        case NB_KEY_3: return ImGuiKey_3;
        case NB_KEY_4: return ImGuiKey_4;
        case NB_KEY_5: return ImGuiKey_5;
        case NB_KEY_6: return ImGuiKey_6;
        case NB_KEY_7: return ImGuiKey_7;
        case NB_KEY_8: return ImGuiKey_8;
        case NB_KEY_9: return ImGuiKey_9;
        case NB_KEY_A: return ImGuiKey_A;
        case NB_KEY_B: return ImGuiKey_B;
        case NB_KEY_C: return ImGuiKey_C;
        case NB_KEY_D: return ImGuiKey_D;
        case NB_KEY_E: return ImGuiKey_E;
        case NB_KEY_F: return ImGuiKey_F;
        case NB_KEY_G: return ImGuiKey_G;
        case NB_KEY_H: return ImGuiKey_H;
        case NB_KEY_I: return ImGuiKey_I;
        case NB_KEY_J: return ImGuiKey_J;
        case NB_KEY_K: return ImGuiKey_K;
        case NB_KEY_L: return ImGuiKey_L;
        case NB_KEY_M: return ImGuiKey_M;
        case NB_KEY_N: return ImGuiKey_N;
        case NB_KEY_O: return ImGuiKey_O;
        case NB_KEY_P: return ImGuiKey_P;
        case NB_KEY_Q: return ImGuiKey_Q;
        case NB_KEY_R: return ImGuiKey_R;
        case NB_KEY_S: return ImGuiKey_S;
        case NB_KEY_T: return ImGuiKey_T;
        case NB_KEY_U: return ImGuiKey_U;
        case NB_KEY_V: return ImGuiKey_V;
        case NB_KEY_W: return ImGuiKey_W;
        case NB_KEY_X: return ImGuiKey_X;
        case NB_KEY_Y: return ImGuiKey_Y;
        case NB_KEY_Z: return ImGuiKey_Z;
        case NB_KEY_F1: return ImGuiKey_F1;
        case NB_KEY_F2: return ImGuiKey_F2;
        case NB_KEY_F3: return ImGuiKey_F3;
        case NB_KEY_F4: return ImGuiKey_F4;
        case NB_KEY_F5: return ImGuiKey_F5;
        case NB_KEY_F6: return ImGuiKey_F6;
        case NB_KEY_F7: return ImGuiKey_F7;
        case NB_KEY_F8: return ImGuiKey_F8;
        case NB_KEY_F9: return ImGuiKey_F9;
        case NB_KEY_F10: return ImGuiKey_F10;
        case NB_KEY_F11: return ImGuiKey_F11;
        case NB_KEY_F12: return ImGuiKey_F12;
        case NB_KEY_F13: return ImGuiKey_F13;
        case NB_KEY_F14: return ImGuiKey_F14;
        case NB_KEY_F15: return ImGuiKey_F15;
        case NB_KEY_F16: return ImGuiKey_F16;
        case NB_KEY_F17: return ImGuiKey_F17;
        case NB_KEY_F18: return ImGuiKey_F18;
        case NB_KEY_F19: return ImGuiKey_F19;
        case NB_KEY_F20: return ImGuiKey_F20;
        case NB_KEY_F21: return ImGuiKey_F21;
        case NB_KEY_F22: return ImGuiKey_F22;
        case NB_KEY_F23: return ImGuiKey_F23;
        case NB_KEY_F24: return ImGuiKey_F24;
        case NB_KEY_AC_BACK: return ImGuiKey_AppBack;
        case NB_KEY_AC_FORWARD: return ImGuiKey_AppForward;
        default: break;
        }
        return ImGuiKey_None;
    }

    void WindowsImGuiPlatform::CreateImGuiWindow(ImGuiViewport* viewport)
    {
        auto* platform_data = static_cast<PlatformData*>(ImGui::GetIO().BackendPlatformUserData);
        auto* viewport_data = new ViewportData;
        viewport->PlatformUserData = viewport_data;

        viewport_data->ParentWindow = GetWindowFromViewportID(viewport->ParentViewportId);

        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        auto* main_viewport_data = static_cast<ViewportData*>(main_viewport->PlatformUserData);

        // Share GL resources with main context
        bool use_opengl = (main_viewport_data->GLContext != nullptr);
        SDL_GLContext backup_context = nullptr;
        if (use_opengl)
        {
            backup_context = SDL_GL_GetCurrentContext();
            SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
            SDL_GL_MakeCurrent(main_viewport_data->Window, main_viewport_data->GLContext);
        }

        WindowProps props = WindowProps();
        props.Title = "No Title Yet";
        props.Width = viewport->Size.x;
        props.Height = viewport->Size.y;
        props.isResizable = !(viewport->Flags & ImGuiViewportFlags_NoDecoration);
        props.AlwaysOnTop = (viewport->Flags & ImGuiViewportFlags_TopMost);
        props.HighDPI = (SDL_GetWindowFlags(SDL_GetWindowFromID(platform_data->windowID)) & SDL_WINDOW_HIGH_PIXEL_DENSITY) == SDL_WINDOW_HIGH_PIXEL_DENSITY;
        props.isUtility = (viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon);
        props.Border = !(viewport->Flags & ImGuiViewportFlags_NoDecoration);
        props.API = RendererAPI::GetAPI();
        Ref<Window> window = Window::Create(props);
        window->SetEventCallback(s_EventCallback);
        viewport_data->Window = static_cast<SDL_Window*>(window->GetNativeWindow());
        viewport_data->WindowID = SDL_GetWindowID(viewport_data->Window);

        SDL_SetWindowParent(viewport_data->Window, viewport_data->ParentWindow);
        SDL_SetWindowPosition(viewport_data->Window, static_cast<int>(viewport->Pos.x), static_cast<int>(viewport->Pos.y));
        viewport_data->WindowOwned = true;
        if (use_opengl)
        {
            viewport_data->GLContext = SDL_GL_GetCurrentContext();
            SDL_GL_SetSwapInterval(0);
        }
        if (use_opengl && backup_context)
            SDL_GL_MakeCurrent(viewport_data->Window, backup_context);

        viewport->PlatformHandle = reinterpret_cast<void*>(static_cast<intptr_t>(viewport_data->WindowID));
        viewport->PlatformHandleRaw = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(SDL_GetWindowFromID(platform_data->windowID)), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
    }

    void WindowsImGuiPlatform::DestroyImGuiWindow(ImGuiViewport* viewport)
    {
        if (auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData))
        {
            if (viewport_data->GLContext && viewport_data->WindowOwned)
                SDL_GL_DestroyContext(viewport_data->GLContext);
            if (viewport_data->Window && viewport_data->WindowOwned)
            {
                Window::DestroyWindow(viewport_data->WindowID);
            }
            viewport_data->GLContext = nullptr;
            viewport_data->Window = nullptr;
            delete viewport_data;
        }
        viewport->PlatformUserData = viewport->PlatformHandle = nullptr;
    }

    void WindowsImGuiPlatform::ShowWindow(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        if (viewport_data->Window) SDL_ShowWindow(viewport_data->Window);
    }

    void WindowsImGuiPlatform::UpdateWIndow(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);

        // Update SDL3 parent if it changed _after_ creation.
        // This is for advanced apps that are manipulating ParentViewportID manually.
        SDL_Window* new_parent = SDL_GetWindowFromID(viewport->ParentViewportId);
        if (new_parent != viewport_data->ParentWindow)
        {
            viewport_data->ParentWindow = new_parent;
            SDL_SetWindowParent(viewport_data->Window, viewport_data->ParentWindow);
        }
    }

    ImVec2 WindowsImGuiPlatform::GetWindowPos(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        int x = 0, y = 0;
        SDL_GetWindowPosition(viewport_data->Window, &x, &y);
        return ImVec2(static_cast<float>(x), static_cast<float>(y));
    }

    void WindowsImGuiPlatform::SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        SDL_SetWindowPosition(viewport_data->Window, static_cast<int>(pos.x), static_cast<int>(pos.y));
    }

    ImVec2 WindowsImGuiPlatform::GetWindowSize(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        int w = 0, h = 0;
        SDL_GetWindowSize(viewport_data->Window, &w, &h);
        return ImVec2(static_cast<float>(w), static_cast<float>(h));
    }

    bool WindowsImGuiPlatform::GetWindowFocus(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        return (SDL_GetWindowFlags(viewport_data->Window) & SDL_WINDOW_INPUT_FOCUS) != 0;
    }

    void WindowsImGuiPlatform::SetWindowFocus(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        SDL_RaiseWindow(viewport_data->Window);
    }

    bool WindowsImGuiPlatform::GetWindowMinimized(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        return (SDL_GetWindowFlags(viewport_data->Window) & SDL_WINDOW_MINIMIZED) != 0;
    }

    void WindowsImGuiPlatform::SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        SDL_SetWindowSize(viewport_data->Window, static_cast<int>(size.x), static_cast<int>(size.y));
    }

    void WindowsImGuiPlatform::SetWindowTitle(ImGuiViewport* viewport, const char* title)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        SDL_SetWindowTitle(viewport_data->Window, title);
    }

    void WindowsImGuiPlatform::RenderWindow(ImGuiViewport* viewport, void*)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        if (viewport_data->GLContext)
            SDL_GL_MakeCurrent(viewport_data->Window, viewport_data->GLContext);
    }

    void WindowsImGuiPlatform::SwapBuffers(ImGuiViewport* viewport, void*)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        if (viewport_data->GLContext)
        {
            SDL_GL_MakeCurrent(viewport_data->Window, viewport_data->GLContext);
            SDL_GL_SwapWindow(viewport_data->Window);
        }
    }

    void WindowsImGuiPlatform::SetWindowAlpha(ImGuiViewport* viewport, float alpha)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        SDL_SetWindowOpacity(viewport_data->Window, alpha);
    }

}
