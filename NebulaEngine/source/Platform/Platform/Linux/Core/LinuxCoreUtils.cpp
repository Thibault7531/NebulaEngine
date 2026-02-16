#include "Platform/Nebula/CoreSystems/Core/CoreUtils.h"

#include <SDL3/SDL.h>

namespace Nebula::Platform
{
    void CoreUtils::InitThread()
    {
        SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
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
