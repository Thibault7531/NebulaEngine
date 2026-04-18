#pragma once

#include "Window.h"
#include "Platform/Nebula/CoreSystems/Events/KeyEvent.h"
#include "Platform/Nebula/CoreSystems/Events/MouseEvent.h"
#include "Platform/Nebula/CoreSystems/Events/WindowEvents.h"

namespace Nebula::Platform
{

    class EventHandler
    {
    public:
        EventHandler(Window* window)
            : m_Window{window} {}
        ~EventHandler() = default;

        void OnWindowResized(WindowResizedEvent& e);
        void OnWindowClosed(WindowClosedEvent& e);
        void OnWindowMinimized(WindowMinimizedEvent& e);
        void OnWindowMaximized(WindowMaximizedEvent& e);
        void OnWindowRestored(WindowRestoredEvent& e);
        void OnWindowMoved(WindowMovedEvent& e);
        void OnWindowFocused(WindowFocusedEvent& e);
        void OnWindowLostFocus(WindowLostFocusEvent& e);

        void OnMouseButtonPressed(MouseButtonPressedEvent& e);
        void OnMouseButtonReleased(MouseButtonReleasedEvent& e);
        void OnMouseMoved(MouseMovedEvent& e);
        void OnMouseScrolled(MouseScrolledEvent& e);
        void OnMouseEntered(MouseEnteredEvent& e);
        void OnMouseLeft(MouseLeftEvent& e);

        void OnKeyPressed(KeyPressedEvent& e);
        void OnKeyReleased(KeyReleasedEvent& e);
        void OnKeyTyped(KeyTypedEvent& e);

    private:
        Window* m_Window;
    };

}
