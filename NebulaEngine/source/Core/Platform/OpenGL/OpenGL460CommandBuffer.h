#pragma once

#include "Nebula/Graphics/CommandBuffer.h"

namespace Nebula::Graphics
{
    class OpenGL460CommandBuffer : public CommandBuffer
    {
    public:
        OpenGL460CommandBuffer() = default;
        ~OpenGL460CommandBuffer() = default;

        virtual Ref<RenderPass> CreateRenderPass(Ref<Framebuffer> framebuffer, bool clear) override;

        virtual void Commit() override {}
    };
}
