#pragma once

#include "Semaphore.h"
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
        virtual Ref<Semaphore> GetImageAcquiredSemaphore() = 0;
        virtual void Present(Ref<Semaphore> renderCompleteSemaphore) = 0;
    };
}
