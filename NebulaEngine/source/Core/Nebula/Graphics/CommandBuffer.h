#pragma once

#include "RenderPass.h"
#include "Buffers/Framebuffer.h"

namespace Nebula::Graphics
{
    class CommandBuffer
    {
    public:
        virtual ~CommandBuffer() = default;

        virtual void Begin() = 0;
        virtual void End() = 0;

        virtual Ref<RenderPass> CreateRenderPass(Ref<Framebuffer> framebuffer, bool clear) = 0;
    };
}
