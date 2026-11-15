#include "ImGuiPlatform.h"

#include "../Graphics/Core/RendererAPI.h"
#include "Nebula/CoreSystems/Application/Application.h"
#include "Platform/Nebula/CoreSystems/Input/Input.h"
#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"


namespace Nebula
{

    void ImGuiPlatform::Init(const Ref<Platform::Window>& window)
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

        // Viewports not supported on linux because wayland does not allow reading or writing window position
#if defined(NB_PLATFORM_WINDOWS) || defined(NB_PLATFORM_MACOS)
        io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
#endif

        // Sets backendio
        platform_io.Platform_SetClipboardTextFn = SetClipboardText;
        platform_io.Platform_GetClipboardTextFn = GetClipboardText;

        // UpdateMonitors()
        UpdateMonitors();

        // Load mouse cursors
        platform_data.MouseCursors[ImGuiMouseCursor_Arrow] = CreateRef<Platform::Cursor>(Platform::CursorType::Arrow);
        platform_data.MouseCursors[ImGuiMouseCursor_TextInput] = CreateRef<Platform::Cursor>(Platform::CursorType::TextInput);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeAll] = CreateRef<Platform::Cursor>(Platform::CursorType::Move);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeNS] = CreateRef<Platform::Cursor>(Platform::CursorType::ResizeNS);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeEW] = CreateRef<Platform::Cursor>(Platform::CursorType::ResizeEW);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeNESW] = CreateRef<Platform::Cursor>(Platform::CursorType::ResizeNESW);
        platform_data.MouseCursors[ImGuiMouseCursor_ResizeNWSE] = CreateRef<Platform::Cursor>(Platform::CursorType::ResizeNWSE);
        platform_data.MouseCursors[ImGuiMouseCursor_Hand] = CreateRef<Platform::Cursor>(Platform::CursorType::Hand);
        platform_data.MouseCursors[ImGuiMouseCursor_NotAllowed] = CreateRef<Platform::Cursor>(Platform::CursorType::NotAllowed);

        // SetupPlatformHandles
        ImGuiViewport* mainViewport = ImGui::GetMainViewport();
        mainViewport->PlatformHandle = reinterpret_cast<void*>(static_cast<intptr_t>(platform_data.Window->GetID()));
        mainViewport->PlatformHandleRaw = platform_data.Window->GetHandle();

        // initMultipleViewports
        initMultipleViewports();
    }

    void ImGuiPlatform::Shutdown() const
    {
        NB_PROFILE_FUNCTION();
        NB_CORE_DEBUG("Shutting down {0}", io.BackendPlatformName);

        ShutdownMultipleViewports();

        io.BackendPlatformName = nullptr;
        io.BackendPlatformUserData = nullptr;
        io.BackendFlags &= ~(ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_HasGamepad | ImGuiBackendFlags_PlatformHasViewports | ImGuiBackendFlags_HasMouseHoveredViewport);
    }

    void ImGuiPlatform::NewFrame(Timestep ts)
    {
        NB_PROFILE_FUNCTION();
        // Setup display size (every frame to accommodate for window resizing)
        auto [w, h] = platform_data.Window->GetClientSize();
        auto [display_w, display_h] = platform_data.Window->GetClientSize();
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

        if (io.WantTextInput)
        {
            Platform::WindowManager::Get().GetMainWindow()->StartInput();
        }
        else
        {
            Platform::WindowManager::Get().GetMainWindow()->StopInput();
        }

        UpdateMouseData();
        UpdateMouseCursor();
    }

    void ImGuiPlatform::OnEvent(Platform::Event& event)
    {
        Platform::EventDispatcher dispatcher(event);

        dispatcher.Dispatch<Platform::MouseMovedEvent>(NB_BIND_EVENT_FN(OnMouseMovedEvent));
        dispatcher.Dispatch<Platform::MouseScrolledEvent>(NB_BIND_EVENT_FN(OnMouseScrolledEvent));
        dispatcher.Dispatch<Platform::MouseButtonPressedEvent>(NB_BIND_EVENT_FN(OnMouseButtonPressedEvent));
        dispatcher.Dispatch<Platform::MouseButtonReleasedEvent>(NB_BIND_EVENT_FN(OnMouseButtonReleasedEvent));
        dispatcher.Dispatch<Platform::MouseEnteredEvent>(NB_BIND_EVENT_FN(OnMouseEnteredEvent));
        dispatcher.Dispatch<Platform::MouseLeftEvent>(NB_BIND_EVENT_FN(OnMouseLeftEvent));
        dispatcher.Dispatch<Platform::KeyTypedEvent>(NB_BIND_EVENT_FN(OnKeyTypedEvent));
        dispatcher.Dispatch<Platform::KeyPressedEvent>(NB_BIND_EVENT_FN(OnKeyPressedEvent));
        dispatcher.Dispatch<Platform::KeyReleasedEvent>(NB_BIND_EVENT_FN(OnKeyReleasedEvent));
        dispatcher.Dispatch<Platform::MonitorsChangedEvent>(NB_BIND_EVENT_FN(OnMonitorsChangedEvent));
        dispatcher.Dispatch<Platform::WindowFocusedEvent>(NB_BIND_EVENT_FN(OnWindowFocusedEvent));
        dispatcher.Dispatch<Platform::WindowLostFocusEvent>(NB_BIND_EVENT_FN(OnWindowLostFocusEvent));
        dispatcher.Dispatch<Platform::WindowClosedEvent>(NB_BIND_EVENT_FN(OnWindowClosedEvent));
        dispatcher.Dispatch<Platform::WindowMovedEvent>(NB_BIND_EVENT_FN(OnWindowMovedEvent));
        dispatcher.Dispatch<Platform::WindowResizedEvent>(NB_BIND_EVENT_FN(OnWindowResizedEvent));
    }

    void ImGuiPlatform::initMultipleViewports() const
    {
        NB_PROFILE_FUNCTION();

        // Register platform interface (will be coupled with a renderer interface)
        platform_io.Platform_CreateWindow = CreateImGuiWindow;
        platform_io.Platform_DestroyWindow = DestroyImGuiWindow;
        platform_io.Platform_ShowWindow = ShowWindow;
        platform_io.Platform_UpdateWindow = UpdateWindow;
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
        viewport_data->GraphicsSurface = platform_data.Window->GetSurface();
        viewport_data->Swapchain = Application::Get().GetMainWindowSwapchain();

        uint32_t maxFramesInFlight = viewport_data->Swapchain->GetMaxFramesInFlight();
        viewport_data->CommandBuffers.resize(maxFramesInFlight);
        viewport_data->VertexBuffers.resize(maxFramesInFlight);
        viewport_data->IndexBuffers.resize(maxFramesInFlight);
        for (uint32_t i = 0; i < maxFramesInFlight; i++)
        {
            viewport_data->CommandBuffers[i] = Application::Get().GetDevice()->GetGraphicsQueue()->CreateCommandBuffer();
        }
        viewport_data->UniformBuffer = Application::Get().GetDevice()->CreateUniformBuffer({
            {Graphics::ShaderDataType::Matrix4, "u_ProjectionMatrix"}
        });

        main_viewport->PlatformUserData = viewport_data;
        main_viewport->PlatformHandle = reinterpret_cast<void*>(static_cast<intptr_t>(platform_data.Window->GetID()));
    }

    void ImGuiPlatform::ShutdownMultipleViewports() const
    {
        ImGui::DestroyPlatformWindows();
    }

    bool ImGuiPlatform::OnMouseMovedEvent(const Platform::MouseMovedEvent& event) const
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.WindowID))) == nullptr) return false;

        ImVec2 mouse_pos(event.GetX(), event.GetY());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            Ref<Platform::Window> window = Platform::WindowManager::Get().GetWindowFromID(event.WindowID);
            auto [window_x, window_y] = window->GetClientPos();
            mouse_pos.x += window_x;
            mouse_pos.y += window_y;
        }
        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMousePosEvent(mouse_pos.x, mouse_pos.y);

        return false;
    }

    bool ImGuiPlatform::OnMouseScrolledEvent(const Platform::MouseScrolledEvent& event) const
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.WindowID))) == nullptr) return false;

        float wheel_x = event.GetXOffset();
        float wheel_y = event.GetYOffset();
        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMouseWheelEvent(wheel_x, wheel_y);
        return false;
    }

    bool ImGuiPlatform::OnMouseButtonPressedEvent(const Platform::MouseButtonPressedEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.WindowID))) == nullptr) return false;

        int mouse_button = -1;
        if (event.GetMouseButton() == MouseButton::Left) { mouse_button = 0; }
        if (event.GetMouseButton() == MouseButton::Right) { mouse_button = 1; }
        if (event.GetMouseButton() == MouseButton::Middle) { mouse_button = 2; }
        if (event.GetMouseButton() == MouseButton::X1) { mouse_button = 3; }
        if (event.GetMouseButton() == MouseButton::X2) { mouse_button = 4; }
        if (mouse_button == -1)
            return false;

        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMouseButtonEvent(mouse_button, true);
        platform_data.MouseButtonsDown = platform_data.MouseButtonsDown | (1 << mouse_button);
        return false;
    }

    bool ImGuiPlatform::OnMouseButtonReleasedEvent(const Platform::MouseButtonReleasedEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.WindowID))) == nullptr) return false;

        int mouse_button = -1;
        if (event.GetMouseButton() == MouseButton::Left) { mouse_button = 0; }
        if (event.GetMouseButton() == MouseButton::Right) { mouse_button = 1; }
        if (event.GetMouseButton() == MouseButton::Middle) { mouse_button = 2; }
        if (event.GetMouseButton() == MouseButton::X1) { mouse_button = 3; }
        if (event.GetMouseButton() == MouseButton::X2) { mouse_button = 4; }
        if (mouse_button == -1)
            return false;

        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMouseButtonEvent(mouse_button, false);
        platform_data.MouseButtonsDown = platform_data.MouseButtonsDown & ~(1 << mouse_button);
        return false;
    }

    bool ImGuiPlatform::OnMouseEnteredEvent(const Platform::MouseEnteredEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.WindowID))) == nullptr) return false;

        platform_data.MouseWindowID = event.WindowID;
        platform_data.MousePendingLeaveFrame = 0;

        return false;
    }

    bool ImGuiPlatform::OnMouseLeftEvent(const Platform::MouseLeftEvent& event)
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.WindowID))) == nullptr) return false;

        if (platform_data.MouseWindowID == event.WindowID) platform_data.MousePendingLeaveFrame = ImGui::GetFrameCount();

        return false;
    }

    bool ImGuiPlatform::OnKeyTypedEvent(const Platform::KeyTypedEvent& event) const
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.WindowID))) == nullptr) return false;

        int keycode = event.GetKeyChar();
        io.AddInputCharacter(keycode);
        return false;
    }

    bool ImGuiPlatform::OnKeyPressedEvent(const Platform::KeyPressedEvent& event) const
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.WindowID))) == nullptr) return false;

        io.AddKeyEvent(ImGuiMod_Ctrl, Platform::Input::IsKeyPressed(KeyboardKey::LeftControl) || Platform::Input::IsKeyPressed(KeyboardKey::RightControl));
        io.AddKeyEvent(ImGuiMod_Shift, Platform::Input::IsKeyPressed(KeyboardKey::LeftShift) || Platform::Input::IsKeyPressed(KeyboardKey::RightShift));
        io.AddKeyEvent(ImGuiMod_Alt, Platform::Input::IsKeyPressed(KeyboardKey::LeftAlt) || Platform::Input::IsKeyPressed(KeyboardKey::RightAlt));
        io.AddKeyEvent(ImGuiMod_Super, Platform::Input::IsKeyPressed(KeyboardKey::LeftGUI) || Platform::Input::IsKeyPressed(KeyboardKey::RightGUI));

        ImGuiKey key = getImGuiKey(event.GetKeyCode());
        io.AddKeyEvent(key, true);
        io.SetKeyEventNativeData(key, static_cast<int>(event.GetKeyCode()), Platform::Input::GetNativeKeyCode(event.GetKeyCode()));

        return false;
    }

    bool ImGuiPlatform::OnKeyReleasedEvent(const Platform::KeyReleasedEvent& event) const
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.WindowID))) == nullptr) return false;

        io.AddKeyEvent(ImGuiMod_Ctrl, Platform::Input::IsKeyPressed(KeyboardKey::LeftControl) || Platform::Input::IsKeyPressed(KeyboardKey::RightControl));
        io.AddKeyEvent(ImGuiMod_Shift, Platform::Input::IsKeyPressed(KeyboardKey::LeftShift) || Platform::Input::IsKeyPressed(KeyboardKey::RightShift));
        io.AddKeyEvent(ImGuiMod_Alt, Platform::Input::IsKeyPressed(KeyboardKey::LeftAlt) || Platform::Input::IsKeyPressed(KeyboardKey::RightAlt));
        io.AddKeyEvent(ImGuiMod_Super, Platform::Input::IsKeyPressed(KeyboardKey::LeftGUI) || Platform::Input::IsKeyPressed(KeyboardKey::RightGUI));

        ImGuiKey key = getImGuiKey(event.GetKeyCode());
        io.AddKeyEvent(key, false);
        io.SetKeyEventNativeData(key, static_cast<int>(event.GetKeyCode()), Platform::Input::GetNativeKeyCode(event.GetKeyCode()));

        return false;
    }

    bool ImGuiPlatform::OnMonitorsChangedEvent(const Platform::MonitorsChangedEvent& event)
    {
        platform_data.WantUpdateMonitors = true;

        return false;
    }

    bool ImGuiPlatform::OnWindowFocusedEvent(const Platform::WindowFocusedEvent& event) const
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.WindowID))) == nullptr) return false;
        Ref<Platform::Window> win = Platform::WindowManager::Get().GetWindowFromID(event.WindowID);

        io.AddFocusEvent(true);
        return false;
    }

    bool ImGuiPlatform::OnWindowLostFocusEvent(const Platform::WindowLostFocusEvent& event) const
    {
        if (ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.WindowID))) == nullptr) return false;

        io.AddFocusEvent(false);
        return false;
    }

    bool ImGuiPlatform::OnWindowClosedEvent(const Platform::WindowClosedEvent& event) const
    {
        ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.WindowID)));
        if (viewport == nullptr)
            return false;

        viewport->PlatformRequestClose = true;

        return false;
    }

    bool ImGuiPlatform::OnWindowMovedEvent(const Platform::WindowMovedEvent& event) const
    {
        ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.WindowID)));
        if (viewport == nullptr)
            return false;

        viewport->PlatformRequestMove = true;

        return false;
    }

    bool ImGuiPlatform::OnWindowResizedEvent(const Platform::WindowResizedEvent& event) const
    {
        ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(event.WindowID)));
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        if (viewport == nullptr)
            return false;

        viewport->PlatformRequestResize = true;
        viewport_data->Swapchain->Resize(event.GetWidth(), event.GetHeight());

        return false;
    }

    void ImGuiPlatform::UpdateMonitors()
    {
        NB_PROFILE_FUNCTION();

        platform_io.Monitors.resize(0);
        platform_data.WantUpdateMonitors = false;

        std::vector<Platform::Display> displays = Platform::Displays::GetDisplays();
        for (int n = 0; n < displays.size(); n++)
        {
            const Platform::Display& display = displays[n];
            ImGuiPlatformMonitor monitor;
            monitor.MainPos = ImVec2(display.x, display.y);
            monitor.MainSize = ImVec2(display.width, display.height);
            monitor.WorkPos = ImVec2(display.workX, display.workY);
            monitor.WorkSize = ImVec2(display.workWidth, display.workHeight);
            monitor.DpiScale = 96;
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
        Ref<Platform::Window> focused_window = Platform::WindowUtils::GetForegroundWindow();
        if (focused_window) focused_window->CaptureMouse(platform_data.MouseButtonsDown != 0);


        if (focused_window && (platform_data.Window == focused_window || ImGui::FindViewportByPlatformHandle(reinterpret_cast<void*>(static_cast<intptr_t>(focused_window->GetID()))) != nullptr))
        {
            // (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when io.ConfigNavMoveSetMousePos is enabled by user)
            if (io.WantSetMousePos)
            {
                if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) Platform::Mouse::SetMousePosGlobal(io.MousePos.x, io.MousePos.y);
                else platform_data.Window->SetMousePos(io.MousePos.x, io.MousePos.y);
            }

            // (Optional) Fallback to provide mouse position when focused (SDL_EVENT_MOUSE_MOTION already provides this when hovered or captured)
            if (platform_data.MouseButtonsDown == 0 || true)
            {
                auto [mouse_x, mouse_y] = Platform::Input::GetMousePosition();
                auto [window_x, window_y] = focused_window->GetClientPos();
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
            Platform::Mouse::HideCursor();
        }
        else
        {
            // Show OS mouse cursor
            Ref<Platform::Cursor> expected_cursor = platform_data.MouseCursors[imgui_cursor] ? platform_data.MouseCursors[imgui_cursor] : platform_data.MouseCursors[ImGuiMouseCursor_Arrow];
            if (platform_data.MouseLastCursor != expected_cursor)
            {
                expected_cursor->Show();
                platform_data.MouseLastCursor = expected_cursor;
            }
            Platform::Mouse::ShowCursor();
        }
    }

    void ImGuiPlatform::SetClipboardText(ImGuiContext*, const char* text)
    {
        Platform::Clipboard::SetText(text);
    }

    const char* ImGuiPlatform::GetClipboardText(ImGuiContext*)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto* backend_data = static_cast<PlatformData*>(io.BackendPlatformUserData);

        backend_data->ClipboardTextData = Platform::Clipboard::GetText();

        return backend_data->ClipboardTextData.c_str();
    }

    Ref<Platform::Window> ImGuiPlatform::GetWindowFromViewportID(ImGuiID viewport_id)
    {
        if (viewport_id != 0)
            if (ImGuiViewport* viewport = ImGui::FindViewportByID(viewport_id))
            {
                return Platform::WindowManager::Get().GetWindowFromID(reinterpret_cast<uint64_t>(viewport->PlatformHandle));
            }
        return nullptr;
    }

    ImGuiKey ImGuiPlatform::getImGuiKey(KeyboardKey key)
    {
        switch (key)
        {
            case KeyboardKey::Tab: return ImGuiKey_Tab;
            case KeyboardKey::LeftArrow: return ImGuiKey_LeftArrow;
            case KeyboardKey::RightArrow: return ImGuiKey_RightArrow;
            case KeyboardKey::UpArrow: return ImGuiKey_UpArrow;
            case KeyboardKey::DownArrow: return ImGuiKey_DownArrow;
            case KeyboardKey::PageUp: return ImGuiKey_PageUp;
            case KeyboardKey::PageDown: return ImGuiKey_PageDown;
            case KeyboardKey::Home: return ImGuiKey_Home;
            case KeyboardKey::End: return ImGuiKey_End;
            case KeyboardKey::Insert: return ImGuiKey_Insert;
            case KeyboardKey::Delete: return ImGuiKey_Delete;
            case KeyboardKey::Backspace: return ImGuiKey_Backspace;
            case KeyboardKey::Space: return ImGuiKey_Space;
            case KeyboardKey::Return: return ImGuiKey_Enter;
            case KeyboardKey::Escape: return ImGuiKey_Escape;
            case KeyboardKey::LeftControl: return ImGuiKey_LeftCtrl;
            case KeyboardKey::LeftShift: return ImGuiKey_LeftShift;
            case KeyboardKey::LeftAlt: return ImGuiKey_LeftAlt;
            case KeyboardKey::LeftGUI: return ImGuiKey_LeftSuper;
            case KeyboardKey::RightControl: return ImGuiKey_RightCtrl;
            case KeyboardKey::RightShift: return ImGuiKey_RightShift;
            case KeyboardKey::RightAlt: return ImGuiKey_RightAlt;
            case KeyboardKey::RightGUI: return ImGuiKey_RightSuper;
            case KeyboardKey::Menu: return ImGuiKey_Menu;
            case KeyboardKey::Num0: return ImGuiKey_0;
            case KeyboardKey::Num1: return ImGuiKey_1;
            case KeyboardKey::Num2: return ImGuiKey_2;
            case KeyboardKey::Num3: return ImGuiKey_3;
            case KeyboardKey::Num4: return ImGuiKey_4;
            case KeyboardKey::Num5: return ImGuiKey_5;
            case KeyboardKey::Num6: return ImGuiKey_6;
            case KeyboardKey::Num7: return ImGuiKey_7;
            case KeyboardKey::Num8: return ImGuiKey_8;
            case KeyboardKey::Num9: return ImGuiKey_9;
            case KeyboardKey::A: return ImGuiKey_A;
            case KeyboardKey::B: return ImGuiKey_B;
            case KeyboardKey::C: return ImGuiKey_C;
            case KeyboardKey::D: return ImGuiKey_D;
            case KeyboardKey::E: return ImGuiKey_E;
            case KeyboardKey::F: return ImGuiKey_F;
            case KeyboardKey::G: return ImGuiKey_G;
            case KeyboardKey::H: return ImGuiKey_H;
            case KeyboardKey::I: return ImGuiKey_I;
            case KeyboardKey::J: return ImGuiKey_J;
            case KeyboardKey::K: return ImGuiKey_K;
            case KeyboardKey::L: return ImGuiKey_L;
            case KeyboardKey::M: return ImGuiKey_M;
            case KeyboardKey::N: return ImGuiKey_N;
            case KeyboardKey::O: return ImGuiKey_O;
            case KeyboardKey::P: return ImGuiKey_P;
            case KeyboardKey::Q: return ImGuiKey_Q;
            case KeyboardKey::R: return ImGuiKey_R;
            case KeyboardKey::S: return ImGuiKey_S;
            case KeyboardKey::T: return ImGuiKey_T;
            case KeyboardKey::U: return ImGuiKey_U;
            case KeyboardKey::V: return ImGuiKey_V;
            case KeyboardKey::W: return ImGuiKey_W;
            case KeyboardKey::X: return ImGuiKey_X;
            case KeyboardKey::Y: return ImGuiKey_Y;
            case KeyboardKey::Z: return ImGuiKey_Z;
            case KeyboardKey::F1: return ImGuiKey_F1;
            case KeyboardKey::F2: return ImGuiKey_F2;
            case KeyboardKey::F3: return ImGuiKey_F3;
            case KeyboardKey::F4: return ImGuiKey_F4;
            case KeyboardKey::F5: return ImGuiKey_F5;
            case KeyboardKey::F6: return ImGuiKey_F6;
            case KeyboardKey::F7: return ImGuiKey_F7;
            case KeyboardKey::F8: return ImGuiKey_F8;
            case KeyboardKey::F9: return ImGuiKey_F9;
            case KeyboardKey::F10: return ImGuiKey_F10;
            case KeyboardKey::F11: return ImGuiKey_F11;
            case KeyboardKey::F12: return ImGuiKey_F12;
            case KeyboardKey::F13: return ImGuiKey_F13;
            case KeyboardKey::F14: return ImGuiKey_F14;
            case KeyboardKey::F15: return ImGuiKey_F15;
            case KeyboardKey::F16: return ImGuiKey_F16;
            case KeyboardKey::F17: return ImGuiKey_F17;
            case KeyboardKey::F18: return ImGuiKey_F18;
            case KeyboardKey::F19: return ImGuiKey_F19;
            case KeyboardKey::F20: return ImGuiKey_F20;
            case KeyboardKey::F21: return ImGuiKey_F21;
            case KeyboardKey::F22: return ImGuiKey_F22;
            case KeyboardKey::F23: return ImGuiKey_F23;
            case KeyboardKey::F24: return ImGuiKey_F24;
            case KeyboardKey::Apostrophe: return ImGuiKey_Apostrophe;
            case KeyboardKey::Comma: return ImGuiKey_Comma;
            case KeyboardKey::Minus: return ImGuiKey_Minus;
            case KeyboardKey::Period: return ImGuiKey_Period;
            case KeyboardKey::Slash: return ImGuiKey_Slash;
            case KeyboardKey::Semicolon: return ImGuiKey_Semicolon;
            case KeyboardKey::Equals: return ImGuiKey_Equal;
            case KeyboardKey::LeftBracket: return ImGuiKey_LeftBracket;
            case KeyboardKey::Backslash: return ImGuiKey_Backslash;
            case KeyboardKey::RightBracket: return ImGuiKey_RightBracket;
            case KeyboardKey::GraveAccent: return ImGuiKey_GraveAccent;
            case KeyboardKey::CapsLock: return ImGuiKey_CapsLock;
            case KeyboardKey::ScrollLock: return ImGuiKey_ScrollLock;
            case KeyboardKey::NumLock: return ImGuiKey_NumLock;
            case KeyboardKey::PrintScreen: return ImGuiKey_PrintScreen;
            case KeyboardKey::Pause: return ImGuiKey_Pause;
            case KeyboardKey::Keypad0: return ImGuiKey_Keypad0;
            case KeyboardKey::Keypad1: return ImGuiKey_Keypad1;
            case KeyboardKey::Keypad2: return ImGuiKey_Keypad2;
            case KeyboardKey::Keypad3: return ImGuiKey_Keypad3;
            case KeyboardKey::Keypad4: return ImGuiKey_Keypad4;
            case KeyboardKey::Keypad5: return ImGuiKey_Keypad5;
            case KeyboardKey::Keypad6: return ImGuiKey_Keypad6;
            case KeyboardKey::Keypad7: return ImGuiKey_Keypad7;
            case KeyboardKey::Keypad8: return ImGuiKey_Keypad8;
            case KeyboardKey::Keypad9: return ImGuiKey_Keypad9;
            case KeyboardKey::KeypadDecimal: return ImGuiKey_KeypadDecimal;
            case KeyboardKey::KeypadDivide: return ImGuiKey_KeypadDivide;
            case KeyboardKey::KeypadMultiply: return ImGuiKey_KeypadMultiply;
            case KeyboardKey::KeypadMinus: return ImGuiKey_KeypadSubtract;
            case KeyboardKey::KeypadPlus: return ImGuiKey_KeypadAdd;
            case KeyboardKey::KeypadEnter: return ImGuiKey_KeypadEnter;
            case KeyboardKey::KeypadEquals: return ImGuiKey_KeypadEqual;
            case KeyboardKey::ACBack: return ImGuiKey_AppBack;
            case KeyboardKey::ACForward: return ImGuiKey_AppForward;
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

        Platform::WindowProperties props = Platform::WindowProperties();
        props.Title = "No Title Yet";
        props.Width = viewport->Size.x;
        props.Height = viewport->Size.y;
        props.x = viewport->Pos.x;
        props.y = viewport->Pos.y;
        props.isResizable = !(viewport->Flags & ImGuiViewportFlags_NoDecoration);
        props.AlwaysOnTop = (viewport->Flags & ImGuiViewportFlags_TopMost);
        props.isUtility = (viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon);
        props.Style = (viewport->Flags & ImGuiViewportFlags_NoDecoration) ? Platform::WindowStyle::BorderlessFullscreen : Platform::WindowStyle::Default;
        props.API = Platform::WindowManager::Get().GetMainWindow()->GetProperties().API;
        const Ref<Platform::Window> window = Platform::WindowManager::Get().CreateWindow(props);
        window->SetEventCallback(s_EventCallback);
        viewport_data->Window = window;

        viewport_data->Window->SetPos(viewport->Pos.x, viewport->Pos.y);
        viewport_data->WindowOwned = true;
        if (Graphics::RendererAPI::GetAPI() != Platform::GraphicsAPI::None)
        {
            viewport_data->GraphicsSurface = viewport_data->Window->GetSurface();
            Application::Get().GetDevice()->SetSurfaceDevice(viewport_data->GraphicsSurface);
            Graphics::FramebufferSpecification spec = {
                window->GetClientSize().first,
                window->GetClientSize().second
            };
            viewport_data->Swapchain = Application::Get().GetDevice()->CreateSwapchain(viewport_data->GraphicsSurface, Graphics::VSyncMode::Adaptive, spec, 0);

            uint32_t maxFramesInFlight = viewport_data->Swapchain->GetMaxFramesInFlight();
            viewport_data->CommandBuffers.resize(maxFramesInFlight);
            viewport_data->VertexBuffers.resize(maxFramesInFlight);
            viewport_data->IndexBuffers.resize(maxFramesInFlight);
            for (uint32_t i = 0; i < maxFramesInFlight; i++)
            {
                viewport_data->CommandBuffers[i] = Application::Get().GetDevice()->GetGraphicsQueue()->CreateCommandBuffer();
            }
            viewport_data->UniformBuffer = Application::Get().GetDevice()->CreateUniformBuffer({
                {Graphics::ShaderDataType::Matrix4, "u_ProjectionMatrix"}
            });
        }

        viewport->PlatformHandle = reinterpret_cast<void*>(static_cast<intptr_t>(viewport_data->Window->GetID()));
        viewport->PlatformHandleRaw = viewport_data->Window->GetHandle();

        ImGuiIO& io = ImGui::GetIO();
        io.AddFocusEvent(true);
    }

    void ImGuiPlatform::DestroyImGuiWindow(ImGuiViewport* viewport)
    {
        NB_PROFILE_FUNCTION();

        if (auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData))
        {
            Application::Get().GetDevice()->WaitIdle();
            if (viewport_data->Window && viewport_data->WindowOwned)
            {
                Platform::WindowManager::Get().DestroyWindow(viewport_data->Window);
            }
            for (uint32_t i = 0; i < viewport_data->CommandBuffers.size(); i++)
            {
                viewport_data->CommandBuffers[i].reset();
            }
            for (uint32_t i = 0; i < viewport_data->IndexBuffers.size(); i++) {
                viewport_data->IndexBuffers[i].reset();
            }
            for (uint32_t i = 0; i < viewport_data->VertexBuffers.size(); i++) {
                viewport_data->VertexBuffers[i].reset();
            }
            viewport_data->UniformBuffer.reset();
            viewport_data->Swapchain = nullptr;
            viewport_data->GraphicsSurface = nullptr;
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

    void ImGuiPlatform::UpdateWindow(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);

        // Update SDL3 parent if it changed _after_ creation.
        // This is for advanced apps that are manipulating ParentViewportID manually.
        ImGuiViewport* parent_viewport = ImGui::FindViewportByID(viewport->ParentViewportId);
        if (parent_viewport)
        {
            Ref<Platform::Window> new_parent = Platform::WindowManager::Get().GetWindowFromID(reinterpret_cast<uint64_t>(parent_viewport->PlatformHandle));
            if (new_parent != viewport_data->ParentWindow)
            {
                viewport_data->ParentWindow = new_parent;
            }
        }
    }

    ImVec2 ImGuiPlatform::GetWindowPos(ImGuiViewport* viewport)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        auto [x, y] = viewport_data->Window->GetClientPos();
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
        auto [w, h] = viewport_data->Window->GetClientSize();
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
        viewport_data->Window->SetClientSize(size.x, size.y);
    }

    void ImGuiPlatform::SetWindowTitle(ImGuiViewport* viewport, const char* title)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        viewport_data->Window->SetTitle(title);
    }

    void ImGuiPlatform::RenderWindow(ImGuiViewport* viewport, void*)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        if (viewport_data->Swapchain)
            viewport_data->Swapchain->NewFrame();
    }

    void ImGuiPlatform::SwapBuffers(ImGuiViewport* viewport, void*)
    {
        auto* viewport_data = static_cast<ViewportData*>(viewport->PlatformUserData);
        if (viewport_data->Swapchain)
        {
            viewport_data->Swapchain->Present();
        }
    }

    void ImGuiPlatform::SetWindowAlpha(ImGuiViewport* viewport, float alpha)
    {
        return; //Not supported
    }

    std::function<void(Platform::Event&)> ImGuiPlatform::s_EventCallback = nullptr;

}
