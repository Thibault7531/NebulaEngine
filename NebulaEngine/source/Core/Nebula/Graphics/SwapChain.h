#pragma once

#include "Buffers/Framebuffer.h"
#include "Nebula/CoreSystems/Core/Core.h"

namespace Nebula::Graphics
{
    class SwapChain
    {
    public:
        virtual ~SwapChain() = default;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual void NewFrame() = 0;
        virtual Ref<Framebuffer> GetCurrentFrame() = 0;
        virtual void Present() = 0;

        virtual uint32_t GetMaxFramesInFlight() const = 0;
        virtual uint32_t GetCurrentFrameIndex() const = 0;
    };
}
