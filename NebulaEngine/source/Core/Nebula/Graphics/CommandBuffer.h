#pragma once
#include "RenderPass.h"
#include "Buffers/Framebuffer.h"

namespace Nebula::Graphics
{
    class CommandBuffer
    {
    public:
        virtual ~CommandBuffer() = default;

        virtual Ref<RenderPass> CreateRenderPass(Ref<Framebuffer> framebuffer, bool clear) = 0;

        virtual void Commit() = 0;
    };
}
