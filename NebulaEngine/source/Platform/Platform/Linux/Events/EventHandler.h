#pragma once

#include <SDL3/SDL.h>

#include "Platform/Nebula/CoreSystems/Input/KeyCodes.h"
#include "Platform/Nebula/CoreSystems/Input/MouseButtonCodes.h"

namespace Nebula::Platform
{

    class EventHandler
    {
    public:
        static bool OnEvent(void* data, SDL_Event* event);
        static SDL_HitTestResult OnHitTest(SDL_Window* window, const SDL_Point* area, void* data);

    private:
        static KeyboardKey GetKeyboardKeyFromSDLKeycode(SDL_Keycode keycode);
        static MouseButton GetMouseButtonFromSDLButtoncode(Uint8 button);
    };

}
