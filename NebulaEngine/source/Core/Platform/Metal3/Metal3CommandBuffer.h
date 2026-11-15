#pragma once

#include "Metal/Metal.hpp"
#include "Nebula/CoreSystems/Core/Core.h"
#include "Nebula/Graphics/CommandBuffer.h"

namespace Nebula::Graphics
{
    class Metal3CommandBuffer : public CommandBuffer
    {
    public:
        Metal3CommandBuffer(MTL::Device* device, MTL::CommandQueue* commandQueue);
        ~Metal3CommandBuffer();

        virtual void Begin() override;
        virtual void End() override;

        virtual Ref<RenderPass> CreateRenderPass(Ref<Framebuffer> framebuffer, bool clear) override;

    public:
        MTL::CommandBuffer* GetMetalCommandBuffer() const { return m_CommandBuffer; }

    private:
        MTL::Device* m_Device;
        MTL::CommandQueue* m_CommandQueue;
        MTL::CommandBuffer* m_CommandBuffer;
    };
}
