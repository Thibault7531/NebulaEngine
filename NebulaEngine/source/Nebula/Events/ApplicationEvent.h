/**
 * This file describes events related to the application
 **/

#pragma once

#include "Event.h"

namespace Nebula
{
	/**
	 * Event for when a window is resized
	 */
	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		/**
		 * Gets the width of the window from the event.
		 * @return Width of the window after being resized
		 */
		inline unsigned int GetWidth() const { return m_Width; }
		/**
		 * Gets the height of the window from the event.
		 * @return Height of the window after being resized
		 */
		inline unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowsResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	private:
		unsigned int m_Width, m_Height;
	};

	/**
	 * Event for when the user wants to close a window
	 */
	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowMinimizeEvent : public Event
	{
	public:
		WindowMinimizeEvent() = default;

		EVENT_CLASS_TYPE(WindowMinimize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class WindowMaximizeEvent : public Event
	{
	public:
		WindowMaximizeEvent() = default;

		EVENT_CLASS_TYPE(WindowMaximize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppTickEvent : public Event
	{
	public:
		AppTickEvent() = default;

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() = default;

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() = default;

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

}
