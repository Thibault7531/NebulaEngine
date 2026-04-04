#include "Platform/Nebula/CoreSystems/Input/Input.h"

#include "SwiftInput.h"

namespace Nebula::Platform
{

    bool Input::IsKeyPressed(KeyboardKey keycode)
    {
        SwiftInput::IsKeyPressed(keycode);
    }

    bool Input::IsMouseButtonPressed(MouseButton button)
    {
        SwiftInput::IsMouseButtonPressed(button);
    }

    std::pair<float, float> Input::GetMousePosition()
    {
        return {SwiftInput::GetMouseX(), SwiftInput::GetMouseY()};
    }

    float Input::GetMouseX()
    {
        return SwiftInput::GetMouseX();
    }

    float Input::GetMouseY()
    {
        return SwiftInput::GetMouseY();
    }

    int32_t Input::GetNativeKeyCode(KeyboardKey keycode)
    {
        return SwiftInput::GetNativeKeycode(keycode);
    }
}
