#pragma once

#include "Nebula/Graphics/CommandBuffer.h"
#include "VulkanQueue.h"

namespace Nebula::Graphics
{
    class VulkanCommandBuffer : public CommandBuffer
    {
    public:
        VulkanCommandBuffer(Device* device, Queue* queue);
        ~VulkanCommandBuffer() override;

        virtual void Begin() override;
        virtual void End() override;

        virtual Ref<RenderPass> CreateRenderPass(Ref<Framebuffer> framebuffer, bool clear) override;

    public:
        vk::CommandBuffer GetVKCommandBuffer() const { return m_CommandBuffer; }

    private:
        VulkanDevice* m_Device;
        VulkanQueue* m_Queue;

        vk::CommandBuffer m_CommandBuffer;
    };
}