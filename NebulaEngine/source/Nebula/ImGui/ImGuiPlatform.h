#pragma once

#include "Nebula/CoreSystems/Windows/Window.h"
#include "Nebula/Events/Event.h"

namespace Nebula
{

    // TODO: Make this class platform-agnostic
    class ImGuiPlatform
    {
    public:
        virtual ~ImGuiPlatform() {}

        virtual void Init(Window* window) = 0;
        virtual void Shutdown() = 0;

        virtual void NewFrame() = 0;

        virtual void OnEvent(Event& event) = 0;

        virtual void SetInputStatus() = 0;

        static Ref<ImGuiPlatform> Create();
    };

}
