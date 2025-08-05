#pragma once

enum class MouseButton
{
    Unknown = 0,
    Left = 1,
    Right = 2,
    Middle = 3,
    X1 = 4,
    X2 = 5
};

inline std::string to_string(const MouseButton button)
{
    switch (button)
    {
    case MouseButton::Left:
        return "Left";
    case MouseButton::Right:
        return "Right";
    case MouseButton::Middle:
        return "Middle";
    case MouseButton::X1:
        return "X1";
    case MouseButton::X2:
        return "X2";
    default:
        return "Unknown";
    }
}
