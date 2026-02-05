#pragma once

#include <wayland-client.h>

#include "Platform/Nebula/CoreSystems/Input/MouseButtonCodes.h"

namespace Nebula::Platform
{

    class EventHandler
    {
    public:
        static void OnMouseEnterEvent(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t x, wl_fixed_t y);
        static void OnMouseLeaveEvent(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface);
        static void OnMouseMoveEvent(void* data, wl_pointer* pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y);
        static void OnButtonEvent(void* data, wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
        static void OnMouseScrolledEvent(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis, wl_fixed_t value);
        static void OnMouseFrameEvent(void* data, wl_pointer* pointer);
        static void OnMouseAxisSourceEvent(void* data, wl_pointer* pointer, uint32_t axis_source);
        static void OnMouseAxisStopEvent(void* data, wl_pointer* pointer, uint32_t time, uint32_t axis);
        static void OnMouseAxisDiscreteEvent(void* data, wl_pointer* pointer, uint32_t axis, int32_t discrete);

        static wl_surface* s_FocusedSurface;

    private:
        static MouseButton GetMouseButtonFromCode(uint32_t button);
    };

}
