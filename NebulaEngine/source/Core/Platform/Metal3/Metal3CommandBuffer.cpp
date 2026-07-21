#include "Metal3CommandBuffer.h"

#include "Metal3RenderPass.h"

namespace Nebula::Graphics
{
    Metal3CommandBuffer::Metal3CommandBuffer(MTL::Device* device, MTL::CommandQueue* commandQueue)
        : m_Device{device}
    {
        m_CommandBuffer = commandQueue->commandBuffer();
    }

    Metal3CommandBuffer::~Metal3CommandBuffer()
    {
    }

    Ref<RenderPass> Metal3CommandBuffer::CreateRenderPass(Ref<Framebuffer> framebuffer, bool clear)
    {
        return CreateRef<Metal3RenderPass>(m_Device, m_CommandBuffer, std::dynamic_pointer_cast<Metal3Framebuffer>(framebuffer), clear);
    }

    void Metal3CommandBuffer::Commit()
    {
        m_CommandBuffer->commit();
        m_CommandBuffer->waitUntilCompleted();
    }
}
