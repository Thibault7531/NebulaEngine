#pragma once

#include <imgui.h>

namespace Nebula
{

    // TODO: Make this class platform-agnostic
    class ImGuiRenderer
    {
    public:
        virtual ~ImGuiRenderer() {}

        virtual void Init() = 0;
        virtual void Shutdown() = 0;

        virtual void NewFrame() = 0;

        virtual void Render(ImDrawData* draw_data) = 0;

        static Ref<ImGuiRenderer> Create();
    };

}
