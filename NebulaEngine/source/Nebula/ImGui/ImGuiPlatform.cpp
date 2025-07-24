#include "ImGuiPlatform.h"

#include "Nebula/CoreSystems/Input/Input.h"
#include "Nebula/CoreSystems/Input/KeyCodes.h"
#include "Nebula/CoreSystems/Input/MouseButtonCodes.h"
#include "Nebula/CoreSystems/Windows/WindowManager.h"

namespace Nebula
{

    void ImGuiPlatform::Init(const Ref<Window>& window)
    {
        NB_PROFILE_FUNCTION();
        NB_CORE_DEBUG("Initializing Nebula::ImGui::Platform");

        // Bind Event Callback
        s_EventCallback = NB_BIND_EVENT_FN(ImGuiPlatform::OnEvent);

        // Set backend data
        platform_data.Window = window;
        io.BackendPlatformUserData = static_cast<void*>(&platform_data);

        // Set backend flags
        io.BackendPlatformName = "Nebula::ImGui::Platform";
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
        platform_data.MouseCursors[ImGuiMouseCursor_Arrow] = CreateRef<Cursor>(Cursor::CursorType::Arrow);
        platform_data.MouseCursors[ImGuiMouseCursor_TextInput] = CreateRef<Cursor>(Cursor::CursorType::TextInput);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeAll] = CreateRef<Cursor>(Cursor::CursorType::Move);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeNS] = CreateRef<Cursor>(Cursor::CursorType::ResizeNS);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeEW] = CreateRef<Cursor>(Cursor::CursorType::ResizeEW);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeNESW] = CreateRef<Cursor>(Cursor::CursorType::ResizeNESW);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeNWSE] = CreateRef<Cursor>(Cursor::CursorType::ResizeNWSE);
        platform_data.MouseCursors[ImGuiMouseCursor_Hand] = CreateRef<Cursor>(Cursor::CursorType::Hand);
        platform_data.MouseCursors[ImGuiMouseCursor_NotAllowed] = CreateRef<Cursor>(Cursor::CursorType::NotAllowed);

        // SetupPlatformHandles
        ImGuiViewport* mainViewport = ImGui::GetMainViewport();
        mainViewport->PlatformHandle = reinterpret_cast<void*>(static_cast<intptr_t>(platform_data.Window->GetID()));
        mainViewport->PlatformHandleRaw = static_cast<HWND>(platform_data.Window->GetHWND());

        // initMultipleViewports
        initMultipleViewports();
    }

    void ImGuiPlatform::Shutdown() const
    {
        NB_PROFILE_FUNCTION();
        NB_CORE_DEBUG("Shutting down {0}", io.BackendPlatformName);

        ShutdownMultipleViewports();

        delete platform_data.ClipboardTextData;

        io.BackendPlatformName = nullptr;
        io.BackendPlatformUserData = nullptr;
        io.BackendFlags &= ~(ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_HasGamepad | ImGuiBackendFlags_PlatformHasViewports | ImGuiBackendFlags_HasMouseHoveredViewport);
    }

    void ImGuiPlatform::NewFrame(Timestep ts)
    {
        NB_PROFILE_FUNCTION();
        // Setup display size (every frame to accommodate for window resizing)
        auto [w, h] = platform_data.Window->GetSize();
        auto [display_w, display_h] = platform_data.Window->GetPixelSize();
        if (platform_data.Window->IsMinimized()) w = h = 0;
        io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
        if (w > 0 && h > 0) io.DisplayFramebufferScale = ImVec2(static_cast<float>(display_w) / w, static_cast<float>(display_h) / h);

        // Update monitors
        if (platform_data.WantUpdateMonitors) UpdateMonitors();

        io.DeltaTime = ts.GetSeconds() > 0 ? ts.GetSeconds() : (float)(1.0f / 60.0f);

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

    void ImGuiPlatform::OnEvent(Event& event)
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

    void ImGuiPlatform::SetInputStatus() const
    {
        if (io.WantTextInput) {
            platform_data.Window->StartTextInput();
        } else {
            platform_data.Window->EndTextInput();
        }
    }

    void ImGuiPlatform::initMultipleViewports() const
    {
        NB_PROFILE_FUNCTION();

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
        viewport_data->Window = platform_data.Window;
        viewport_data->WindowOwned = false;
        viewport_data->GraphicsContext = platform_data.Window->GetContext();
        main_viewport->PlatformUserData = viewport_data;
        main_viewport->PlatformHandle = reinterpret_cast<void*>(static_cast<intptr_t>(platform_data.Window->GetID()));
    }

    void ImGuiPlatform::ShutdownMultipleViewports() const
    {
        ImGui::DestroyPlatformWindows();
    }

    bool ImGuiPlatform::OnMouseMovedEvent(const MouseMovedEvent& event) const
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.Window->GetID()))) == nullptr) return false;

        ImVec2 mouse_pos(event.GetX(), event.GetY());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            Window* window = event.Window;
            auto [window_x, window_y] = window->GetPos();
            mouse_pos.x += window_x;
            mouse_pos.y += window_y;
        }
        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMousePosEvent(mouse_pos.x, mouse_pos.y);

        return false;
    }

    bool ImGuiPlatform::OnMouseScrolledEvent(const MouseScrolledEvent& event) const
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.Window->GetID()))) == nullptr) return false;

        float wheel_x = event.GetXOffset();
        float wheel_y = event.GetYOffset();
        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMouseWheelEvent(wheel_x, wheel_y);
        return false;
    }

    bool ImGuiPlatform::OnMouseButtonPressedEvent(const MouseButtonPressedEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.Window->GetID()))) == nullptr) return false;

        int mouse_button = -1;
        if (event.GetMouseButton() == NB_BUTTON_LEFT) { mouse_button = 0; }
        if (event.GetMouseButton() == NB_BUTTON_RIGHT) { mouse_button = 1; }
        if (event.GetMouseButton() == NB_BUTTON_MIDDLE) { mouse_button = 2; }
        if (event.GetMouseButton() == NB_BUTTON_X1) { mouse_button = 3; }
        if (event.GetMouseButton() == NB_BUTTON_X2) { mouse_button = 4; }
        if (mouse_button == -1)
            return false;

        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMouseButtonEvent(mouse_button, true);
        platform_data.MouseButtonsDown = platform_data.MouseButtonsDown | (1 << mouse_button);
        return false;
    }

    bool ImGuiPlatform::OnMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.Window->GetID()))) == nullptr) return false;

        int mouse_button = -1;
        if (event.GetMouseButton() == NB_BUTTON_LEFT) { mouse_button = 0; }
        if (event.GetMouseButton() == NB_BUTTON_RIGHT) { mouse_button = 1; }
        if (event.GetMouseButton() == NB_BUTTON_MIDDLE) { mouse_button = 2; }
        if (event.GetMouseButton() == NB_BUTTON_X1) { mouse_button = 3; }
        if (event.GetMouseButton() == NB_BUTTON_X2) { mouse_button = 4; }
        if (mouse_button == -1)
            return false;

        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMouseButtonEvent(mouse_button, false);
        platform_data.MouseButtonsDown = platform_data.MouseButtonsDown & ~(1 << mouse_button);
        return false;
    }

    bool ImGuiPlatform::OnMouseEnteredEvent(const MouseEnteredEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.Window->GetID()))) == nullptr) return false;

        platform_data.MouseWindowID = event.Window->GetID();
        platform_data.MousePendingLeaveFrame = 0;

        return false;
    }

    bool ImGuiPlatform::OnMouseLeftEvent(const MouseLeftEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.Window->GetID()))) == nullptr) return false;

        platform_data.MousePendingLeaveFrame = ImGui::GetFrameCount();

        return false;
    }

    bool ImGuiPlatform::OnKeyTypedEvent(const KeyTypedEvent& event) const
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.Window->GetID()))) == nullptr) return false;

        int keycode = event.GetKeyCode();
        if (keycode > 0 && keycode < 0x10000)
            io.AddInputCharacter(keycode);
        return true;
    }

    bool ImGuiPlatform::OnKeyPressedEvent(const KeyPressedEvent& event) const
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.Window->GetID()))) == nullptr) return false;

        io.AddKeyEvent(ImGuiMod_Ctrl, Input::IsKeyPressed(NB_KEY_LCTRL) || Input::IsKeyPressed(NB_KEY_RCTRL));
        io.AddKeyEvent(ImGuiMod_Shift, Input::IsKeyPressed(NB_KEY_LSHIFT) || Input::IsKeyPressed(NB_KEY_RSHIFT));
        io.AddKeyEvent(ImGuiMod_Alt, Input::IsKeyPressed(NB_KEY_LALT) || Input::IsKeyPressed(NB_KEY_RALT));
        io.AddKeyEvent(ImGuiMod_Super, Input::IsKeyPressed(NB_KEY_LGUI) || Input::IsKeyPressed(NB_KEY_RGUI));

        ImGuiKey key = getImGuiKey(event.GetKeyCode());
        io.AddKeyEvent(key, true);
        io.SetKeyEventNativeData(key, event.GetKeyCode(), Keyboard::GetScancodeFromKeycode(event.GetKeyCode()));

        return false;
    }

    bool ImGuiPlatform::OnKeyReleasedEvent(const KeyReleasedEvent& event) const
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.Window->GetID()))) == nullptr) return false;

        io.AddKeyEvent(ImGuiMod_Ctrl, Input::IsKeyPressed(NB_KEY_LCTRL) || Input::IsKeyPressed(NB_KEY_RCTRL));
        io.AddKeyEvent(ImGuiMod_Shift, Input::IsKeyPressed(NB_KEY_LSHIFT) || Input::IsKeyPressed(NB_KEY_RSHIFT));
        io.AddKeyEvent(ImGuiMod_Alt, Input::IsKeyPressed(NB_KEY_LALT) || Input::IsKeyPressed(NB_KEY_RALT));
        io.AddKeyEvent(ImGuiMod_Super, Input::IsKeyPressed(NB_KEY_LGUI) || Input::IsKeyPressed(NB_KEY_RGUI));

        ImGuiKey key = getImGuiKey(event.GetKeyCode());
        io.AddKeyEvent(key, false);
        io.SetKeyEventNativeData(key, event.GetKeyCode(), Keyboard::GetScancodeFromKeycode(event.GetKeyCode()));

        return false;
    }

    bool ImGuiPlatform::OnMonitorsChangedEvent(const MonitorsChangedEvent& event)
    {
        platform_data.WantUpdateMonitors = true;

        return false;
    }

    bool ImGuiPlatform::OnWindowFocusedEvent(const WindowFocusedEvent& event) const
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.Window->GetID()))) == nullptr) return false;

        io.AddFocusEvent(true);
        return false;
    }

    bool ImGuiPlatform::OnWindowLostFocusEvent(const WindowLostFocusEvent& event) const
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.Window->GetID()))) == nullptr) return false;

        io.AddFocusEvent(false);
        return false;
    }

    bool ImGuiPlatform::OnWindowClosedEvent(const WindowCloseEvent& event) const
    {
        ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.Window->GetID())));
        if (viewport == nullptr)
            return false;

        viewport->PlatformRequestClose = true;

        return true;
    }

    bool ImGuiPlatform::OnWindowMovedEvent(const WindowMovedEvent& event) const
    {
        ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.Window->GetID())));
        if (viewport == nullptr)
            return false;

        viewport->PlatformRequestMove = true;

        return true;
    }

    bool ImGuiPlatform::OnWindowResizedEvent(const WindowResizeEvent& event) const
    {
        ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.Window->GetID())));
        if (viewport == nullptr)
            return false;

        viewport->PlatformRequestResize = true;

        return true;
    }

    void ImGuiPlatform::UpdateMonitors()
    {
        NB_PROFILE_FUNCTION();

        platform_io.Monitors.resize(0);
        platform_data.WantUpdateMonitors = false;

        std::vector<Displays::display> displays = Displays::GetDisplays();
        for (int n = 0; n < displays.size(); n++)
        {
            const Displays::display& display = displays[n];
            ImGuiPlatformMonitor monitor;
            monitor.MainPos = ImVec2(display.x, display.y);
            monitor.MainSize = ImVec2(display.width, display.height);
            monitor.WorkPos = ImVec2(display.workX, display.workY);
            monitor.WorkSize = ImVec2(display.workWidth, display.workHeight);
            monitor.DpiScale = display.dpi;
            monitor.PlatformHandle = reinterpret_cast<void*>(n);
            if (monitor.DpiScale <= 0.0f)
                continue;
            platform_io.Monitors.push_back(monitor);
        }
    }

    void ImGuiPlatform::UpdateMouseData() const
    {
        NB_PROFILE_FUNCTION();

        // We forward mouse input when hovered or captured (via SDL_EVENT_MOUSE_MOTION) or when focused (below)
        // SDL_CaptureMouse() let the OS know e.g. that our imgui drag outside the SDL window boundaries shouldn't e.g. trigger other operations outside
        Mouse::CaptureMouse(platform_data.MouseButtonsDown != 0);
        Ref<Window> focused_window = Keyboard::GetCapturingWindow();


        if (focused_window && (platform_data.Window == focused_window || ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(focused_window->GetID()))) != nullptr))
        {
            // (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when io.ConfigNavMoveSetMousePos is enabled by user)
            if (io.WantSetMousePos)
            {
                if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
                    Mouse::SetMousePosGlobal(io.MousePos.x, io.MousePos.y);
                else
                    platform_data.Window->SetMousePos(io.MousePos.x, io.MousePos.y);
            }

            // (Optional) Fallback to provide mouse position when focused (SDL_EVENT_MOUSE_MOTION already provides this when hovered or captured)
            if (platform_data.MouseButtonsDown == 0)
            {
                auto [mouse_x, mouse_y] = Mouse::GetMousePos();
                auto [window_x, window_y] = focused_window->GetPos();
                if (!(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
                {
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

    void ImGuiPlatform::UpdateMouseCursor()
    {
        NB_PROFILE_FUNCTION();

        if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
            return;

        ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
        if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
        {
            // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
            Mouse::HideCursor();
        }
        else
        {
            // Show OS mouse cursor
            Ref<Cursor> expected_cursor = platform_data.MouseCursors[imgui_cursor] ? platform_data.MouseCursors[imgui_cursor] : platform_data.MouseCursors[ImGuiMouseCursor_Arrow];
            if (platform_data.MouseLastCursor != expected_cursor)
            {
                expected_cursor->Show();
                platform_data.MouseLastCursor = expected_cursor;
            }
            Mouse::ShowCursor();
        }
    }

    void ImGuiPlatform::SetClipboardText(ImGuiContext*, const char* text)
    {
        Clipboard::SetText(text);
    }

    const char* ImGuiPlatform::GetClipboardText(ImGuiContext*)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto* backend_data = static_cast<PlatformData*>(io.BackendPlatformUserData);

        std::string sdl_clipboard_text = Clipboard::GetText();
        backend_data->ClipboardTextData = sdl_clipboard_text.data();

        return backend_data->ClipboardTextData;
    }

    void ImGuiPlatform::SetIMEData(ImGuiContext*, ImGuiViewport* viewport, ImGuiPlatformImeData* IMEData)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto* backend_data = static_cast<PlatformData*>(io.BackendPlatformUserData);
        Ref<Window> window = backend_data->Window;

        if ((IMEData->WantVisible == false || backend_data->ImeWindow != window) && backend_data->ImeWindow != nullptr)
        {
            backend_data->ImeWindow->EndTextInput();
            backend_data->ImeWindow = nullptr;
        }

        if (IMEData->WantVisible)
        {
            window->SetInputArea(IMEData->InputPos.x, IMEData->InputPos.y, 1, IMEData->InputLineHeight);
            window->StartTextInput();

            backend_data->ImeWindow = window;
        }
    }

    Ref<Window> ImGuiPlatform::GetWindowFromViewportID(ImGuiID viewport_id)
    {
        if (viewport_id != 0)
            if (ImGuiViewport* viewport = ImGui::FindViewportByID(viewport_id))
            {
                return WindowManager::Get().GetWindow(reinterpret_cast<uint64_t>(viewport->PlatformHandle));
            }
        return nullptr;
    }

    ImGuiKey ImGuiPlatform::getImGuiKey(int keycode)
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

    void ImGuiPlatform::CreateImGuiWindow(ImGuiViewport* viewport)
    {
        NB_PROFILE_FUNCTION();

        auto* platform_data = static_cast<PlatformData*>(ImGui::GetIO().BackendPlatformUserData);
        auto* viewport_data = new ViewportData;
        viewport->PlatformUserData = viewport_data;

        viewport_data->ParentWindow = GetWindowFromViewportID(viewport->ParentViewportId);

        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        auto* main_viewport_data = static_cast<ViewportData*>(main_viewport->PlatformUserData);

        if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
        {
            main_viewport_data->GraphicsContext->SetCurrent();
        }

        WindowProps props = WindowProps();
        props.Title = "No Title Yet";
        props.Width = viewport->Size.x;
        props.Height = viewport->Size.y;
        props.isResizable = !(viewport->Flags & ImGuiViewportFlags_NoDecoration);
        props.AlwaysOnTop = (viewport->Flags & ImGuiViewportFlags_TopMost);
        props.HighDPI = platform_data->Window->IsHighDPI();
        props.isUtility = (viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon);
        props.Border = !(viewport->Flags & ImGuiViewportFlags_NoDecoration);
        props.API = RendererAPI::GetAPI();
        const Ref<Window> window = WindowManager::Get().CreateNewWindow(props);
        window->SetEventCallback(s_EventCallback);
        viewport_data->Window = window;

        viewport_data->Window->SetParentWindow(viewport_data->ParentWindow);
        viewport_data->Window->SetPos(viewport->Pos.x, viewport->Pos.y);
        viewport_data->WindowOwned = true;
        if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
        {
            viewport_data->GraphicsContext = viewport_data->Window->GetContext();
            viewport_data->Window->SetVSync(false);
            viewport_data->GraphicsContext->SetCurrent();
        }

        viewport->PlatformHandle = reinterpret_cast<void*>(static_cast<intptr_t>(viewport_data->Window->GetID()));
        viewport->PlatformHandleRaw = static_cast<HWND>(viewport_data->Window->GetHWND());
    }

    void ImGuiPlatform::DestroyImGuiWindow(ImGuiViewport* viewport)
    {
        NB_PROFILE_FUNCTION();

        if (auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData))
        {
            if (viewport_data->Window && viewport_data->WindowOwned)
            {
                WindowManager::Get().DestroyWindow(viewport_data->Window);
            }
            viewport_data->GraphicsContext = nullptr;
            viewport_data->Window = nullptr;
            delete viewport_data;
        }
        viewport->PlatformUserData = viewport->PlatformHandle = nullptr;
    }

    void ImGuiPlatform::ShowWindow(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        if (viewport_data->Window) viewport_data->Window->Show();
    }

    void ImGuiPlatform::UpdateWIndow(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);

        // Update SDL3 parent if it changed _after_ creation.
        // This is for advanced apps that are manipulating ParentViewportID manually.
        ImGuiViewport* parent_viewport = ImGui::FindViewportByID(viewport->ParentViewportId);
        Ref<Window> new_parent = WindowManager::Get().GetWindow(reinterpret_cast<uint64_t>(parent_viewport->PlatformHandle));
        if (new_parent != viewport_data->ParentWindow)
        {
            viewport_data->ParentWindow = new_parent;
            viewport_data->Window->SetParentWindow(viewport_data->ParentWindow);
        }
    }

    ImVec2 ImGuiPlatform::GetWindowPos(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        auto [x, y] = viewport_data->Window->GetPos();
        return {static_cast<float>(x), static_cast<float>(y)};
    }

    void ImGuiPlatform::SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        viewport_data->Window->SetPos(pos.x, pos.y);
    }

    ImVec2 ImGuiPlatform::GetWindowSize(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        auto [w, h] = viewport_data->Window->GetSize();
        return {static_cast<float>(w), static_cast<float>(h)};
    }

    bool ImGuiPlatform::GetWindowFocus(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        return viewport_data->Window->IsFocused();
    }

    void ImGuiPlatform::SetWindowFocus(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        viewport_data->Window->Raise();
    }

    bool ImGuiPlatform::GetWindowMinimized(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        return viewport_data->Window->IsMinimized();
    }

    void ImGuiPlatform::SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        viewport_data->Window->SetSize(size.x, size.y);
    }

    void ImGuiPlatform::SetWindowTitle(ImGuiViewport* viewport, const char* title)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        viewport_data->Window->SetTitle(title);
    }

    void ImGuiPlatform::RenderWindow(ImGuiViewport* viewport, void*)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        if (viewport_data->GraphicsContext)
            viewport_data->GraphicsContext->SetCurrent();
    }

    void ImGuiPlatform::SwapBuffers(ImGuiViewport* viewport, void*)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        if (viewport_data->GraphicsContext)
        {
            viewport_data->GraphicsContext->SetCurrent();
            viewport_data->GraphicsContext->SwapBuffers();
        }
    }

    void ImGuiPlatform::SetWindowAlpha(ImGuiViewport* viewport, float alpha)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        viewport_data->Window->SetAlpha(alpha);
    }

    std::function<void(Event&)> ImGuiPlatform::s_EventCallback = nullptr;

}
