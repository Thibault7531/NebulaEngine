#include "VulkanCommandBuffer.h"

#include "VulkanRenderPass.h"

namespace Nebula::Graphics
{
    VulkanCommandBuffer::VulkanCommandBuffer(Device* device, Queue* queue)
        : m_Device{dynamic_cast<VulkanDevice*>(device)}, m_Queue{dynamic_cast<VulkanQueue*>(queue)}
    {
        vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
        commandBufferAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
        commandBufferAllocateInfo.commandPool = m_Queue->GetVKCommandPool();
        commandBufferAllocateInfo.commandBufferCount = 1;

        m_CommandBuffer = m_Device->GetVKLogicalDevice().allocateCommandBuffers(commandBufferAllocateInfo)[0];
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        m_Device->GetVKLogicalDevice().freeCommandBuffers(m_Queue->GetVKCommandPool(), 1, &m_CommandBuffer);
    }

    void VulkanCommandBuffer::Begin()
    {
        m_CommandBuffer.reset();

        vk::CommandBufferBeginInfo commandBufferBeginInfo;
        commandBufferBeginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
        commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        m_CommandBuffer.begin(commandBufferBeginInfo);
    }

    void VulkanCommandBuffer::End()
    {
        m_CommandBuffer.end();
    }

    Ref<RenderPass> VulkanCommandBuffer::CreateRenderPass(Ref<Framebuffer> framebuffer, bool clear)
    {
        return CreateRef<VulkanRenderPass>(m_Device, this, framebuffer, clear);
    }
}
