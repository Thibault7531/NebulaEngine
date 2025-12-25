#pragma once

#include "Event.h"
#include "Platform/Nebula/CoreSystems/Input/KeyCodes.h"

namespace Nebula::Platform
{

    class KeyEvent : public Event
    {
    public:
        inline KeyboardKey GetKeyCode() const { return m_KeyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        KeyEvent(const KeyboardKey keycode)
            : m_KeyCode(keycode) {}

        KeyboardKey m_KeyCode;
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(const KeyboardKey keycode, const uint32_t repeatCount)
            : KeyEvent(keycode), m_RepeatCount(repeatCount) {}

        inline uint32_t GetRepeatCount() const { return m_RepeatCount; }

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << to_string(m_KeyCode) << " (" << m_RepeatCount << " repeats)";
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        uint32_t m_RepeatCount;
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(const KeyboardKey keycode)
            : KeyEvent(keycode) {}

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << to_string(m_KeyCode);
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public Event
    {
    public:
        KeyTypedEvent(const char KeyChar)
            : m_KeyChar(KeyChar) {}

        inline char GetKeyChar() const { return m_KeyChar; }

        virtual std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << m_KeyChar;
            return ss.str();
        }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
        EVENT_CLASS_TYPE(KeyTyped)

    private:
        char m_KeyChar;
    };

}