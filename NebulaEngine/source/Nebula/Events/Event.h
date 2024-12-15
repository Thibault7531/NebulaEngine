#pragma once

#include "nbpch.h"

#include "Nebula/Core.h"

namespace Nebula
{
	/**
	 * Type of the event.
	 */
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	/**
	 * Categories of the event.
	 */
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput       = BIT(1),
		EventCategoryKeyboard    = BIT(2),
		EventCategoryMouse       = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	/**
	 * Basics of an event.
	 */
	class Event
	{
		friend class EventDispatcher;
	public:
		/**
		 * Gets the type of the event.
		 * @return Type of the event
		 */
		virtual EventType GetEventType() const = 0;
		/**
		 * Gets the name of the event.
		 * @return Name of the event
		 */
		virtual const char* GetName() const = 0;
		/**
		 * Gets the category flags of the event.
		 * @return Categories of the event
		 */
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		/**
		 * If the event is in the specified category.
		 * @param category Category to be tested with
		 * @return If the event is in the specified category
		 */
		inline bool IsInCategory(EventCategory category) const
		{
			return GetCategoryFlags() & category;
		}

		bool Handled = false;
	};

	/**
	 * Class to dispatch events depending on their type.
	 */
	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		/**
		 * Dispatches an event depending on its type.
		 * @tparam T Event class
		 * @param func Function to be called with the event
		 * @return If the event has been handled by func
		 */
		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

	inline std::string format_as(const Event& e)
	{
		return e.ToString();
	}

}
