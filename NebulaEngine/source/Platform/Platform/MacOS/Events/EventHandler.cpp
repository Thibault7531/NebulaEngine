#include "EventHandler.h"

#include "Input.h"

namespace Nebula::Platform
{
    void EventHandler::OnWindowResized(WindowResizedEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);

            WindowRenderEvent event{};
            event.WindowID = m_Window->GetID();
            m_Window->GetEventCallback()(event);
        }
    }

    void EventHandler::OnWindowClosed(WindowClosedEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnWindowMinimized(WindowMinimizedEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnWindowMaximized(WindowMaximizedEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnWindowRestored(WindowRestoredEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnWindowMoved(WindowMovedEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnWindowFocused(WindowFocusedEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnWindowLostFocus(WindowLostFocusEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnMouseMoved(MouseMovedEvent& e)
    {
        if (e.GetX() < 0 || e.GetX() > m_Window->GetSize().first || e.GetY() < 0 || e.GetY() > m_Window->GetSize().second) return;

        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnMouseScrolled(MouseScrolledEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnMouseEntered(MouseEnteredEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnMouseLeft(MouseLeftEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnKeyPressed(KeyPressedEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnKeyReleased(KeyReleasedEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }

    void EventHandler::OnKeyTyped(KeyTypedEvent& e)
    {
        std::function<void(Event&)> callback = m_Window->GetEventCallback();
        if (callback)
        {
            e.WindowID = m_Window->GetID();
            callback(e);
        }
    }
}
