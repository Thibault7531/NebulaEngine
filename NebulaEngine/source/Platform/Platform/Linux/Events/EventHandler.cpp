#include "EventHandler.h"

#include <linux/input-event-codes.h>

#include "Platform/Nebula/CoreSystems/Core/Log.h"
#include "Platform/Nebula/CoreSystems/Events/MouseEvent.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"
#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"

namespace Nebula::Platform
{
    wl_surface* EventHandler::s_FocusedSurface = nullptr;

    MouseButton EventHandler::GetMouseButtonFromCode(uint32_t button)
    {
        switch (button)
        {
        case BTN_LEFT: return MouseButton::Left;
        case BTN_RIGHT: return MouseButton::Right;
        case BTN_MIDDLE: return MouseButton::Middle;
        case BTN_EXTRA: return MouseButton::X1;
        case BTN_SIDE: return MouseButton::X2;
        default: return MouseButton::Unknown;
        }
    }

    void EventHandler::OnMouseEnterEvent(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t x, wl_fixed_t y)
    {
        s_FocusedSurface = surface;
        Ref<Window> window = WindowManager::Get().GetWindowFromHandle(surface);

        MouseEnteredEvent event{};
        event.WindowID = window->GetID();
        window->GetEventCallback()(event);
    }

    void EventHandler::OnMouseLeaveEvent(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface)
    {
        Ref<Window> window = WindowManager::Get().GetWindowFromHandle(surface);

        MouseLeftEvent event{};
        event.WindowID = window->GetID();
        window->GetEventCallback()(event);
    }

    void EventHandler::OnMouseMoveEvent(void* data, wl_pointer* pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y)
    {
        Ref<Window> window = WindowManager::Get().GetWindowFromHandle(s_FocusedSurface);

        MouseMovedEvent event{static_cast<float>(wl_fixed_to_int(x)), static_cast<float>(wl_fixed_to_int(y))};
        event.WindowID = window->GetID();
        window->GetEventCallback()(event);
    }

    void EventHandler::OnButtonEvent(void* data, wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
    {
        Ref<Window> window = WindowManager::Get().GetWindowFromHandle(s_FocusedSurface);

        if (state == WL_POINTER_BUTTON_STATE_PRESSED)
        {
            MouseButtonPressedEvent event = MouseButtonPressedEvent(GetMouseButtonFromCode(button));
            event.WindowID = window->GetID();
            window->GetEventCallback()(event);
        } else
        {
            MouseButtonReleasedEvent event = MouseButtonReleasedEvent(GetMouseButtonFromCode(button));
            event.WindowID = window->GetID();
            window->GetEventCallback()(event);
        }
    }

    void EventHandler::OnMouseScrolledEvent(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value)
    {
        Ref<Window> window = WindowManager::Get().GetWindowFromHandle(s_FocusedSurface);

        if (axis == WL_POINTER_AXIS_VERTICAL_SCROLL)
        {
            MouseScrolledEvent event = MouseScrolledEvent(0, static_cast<float>(wl_fixed_to_int(value)));
            event.WindowID = window->GetID();
            window->GetEventCallback()(event);
        } else
        {
            MouseScrolledEvent event = MouseScrolledEvent(static_cast<float>(wl_fixed_to_int(value)), 0);
            event.WindowID = window->GetID();
            window->GetEventCallback()(event);
        }
    }

    void EventHandler::OnMouseFrameEvent(void* data, wl_pointer* pointer) {}
    void EventHandler::OnMouseAxisSourceEvent(void* data, wl_pointer* pointer, uint32_t axis_source) {}
    void EventHandler::OnMouseAxisStopEvent(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis) {}
    void EventHandler::OnMouseAxisDiscreteEvent(void* data, wl_pointer* pointer, uint32_t axis, int32_t discrete) {}
}
