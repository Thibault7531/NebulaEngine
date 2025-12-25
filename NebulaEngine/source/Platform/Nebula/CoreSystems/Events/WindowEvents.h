#pragma once

#include "Event.h"

namespace Nebula::Platform
{

    class WindowResizedEvent : public Event
    {
    public:
        WindowResizedEvent(const uint32_t width, const uint32_t height)
            : m_Width(width), m_Height(height) {}

        [[nodiscard]] inline uint32_t GetWidth() const { return m_Width; }
        [[nodiscard]] inline uint32_t GetHeight() const { return m_Height; }

        [[nodiscard]] virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResized)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        uint32_t m_Width, m_Height;
    };

    class WindowClosedEvent : public Event
    {
    public:
        WindowClosedEvent() = default;

        EVENT_CLASS_TYPE(WindowClosed)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class WindowMinimizedEvent : public Event
    {
    public:
        WindowMinimizedEvent() = default;

        EVENT_CLASS_TYPE(WindowMinimized)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class WindowMaximizedEvent : public Event
    {
    public:
        WindowMaximizedEvent() = default;

        EVENT_CLASS_TYPE(WindowMaximized)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class WindowRestoredEvent : public Event
    {
    public:
        WindowRestoredEvent() = default;

        EVENT_CLASS_TYPE(WindowRestored)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class WindowMovedEvent : public Event
    {
    public:
        WindowMovedEvent(const int32_t x, const int32_t y)
            : m_X(x), m_Y(y) {}

        [[nodiscard]] inline int32_t GetX() const { return m_X; }
        [[nodiscard]] inline int32_t GetY() const { return m_Y; }

        EVENT_CLASS_TYPE(WindowMoved)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        int32_t m_X, m_Y;
    };

    class WindowFocusedEvent : public Event
    {
    public:
        WindowFocusedEvent() = default;

        EVENT_CLASS_TYPE(WindowFocused)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class WindowLostFocusEvent : public Event
    {
    public:
        WindowLostFocusEvent() = default;

        EVENT_CLASS_TYPE(WindowLostFocus)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class MonitorsChangedEvent : public Event
    {
    public:
        MonitorsChangedEvent() = default;

        EVENT_CLASS_TYPE(MonitorsChanged)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

}