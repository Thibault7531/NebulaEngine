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

        virtual Ref<RenderPass> CreateRenderPass(Ref<Framebuffer> framebuffer, bool clear) override;

        virtual void Commit() override;

    private:
        MTL::Device* m_Device;
        MTL::CommandBuffer* m_CommandBuffer;
    };
}
