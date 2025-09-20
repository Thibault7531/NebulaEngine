#pragma once
#include "KeyCodes.h"
#include "MouseButtonCodes.h"

namespace Nebula {

    class Input
    {
    public:
        static bool IsKeyPressed(KeyboardKey keycode);

        static bool IsMouseButtonPressed(MouseButton button);
        static std::pair<float, float> GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();

    };

}
