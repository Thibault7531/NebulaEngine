#include "Platform/Nebula/CoreSystems/Core/CoreUtils.h"

#include <SDL3/SDL.h>

#include "Platform/Nebula/CoreSystems/Core/Log.h"

namespace Nebula::Platform
{
    void CoreUtils::InitThread()
    {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "wayland");
        if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) NB_PLATFORM_CRITICAL("Failed to initialize SDL: {0}", SDL_GetError());
    }

    void CoreUtils::InitThread_MT()
    {
    }

    void CoreUtils::PumpEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0);
    }
}
