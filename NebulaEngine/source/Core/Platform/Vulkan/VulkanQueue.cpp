#include "VulkanQueue.h"
#include "VulkanCommandBuffer.h"

namespace Nebula::Graphics
{
    VulkanQueue::VulkanQueue(Device* device, QueueType type)
        : m_Device{dynamic_cast<VulkanDevice*>(device)}, m_Type{type}
    {
        uint32_t queueFamily = 0;
        switch (type)
        {
        case QueueType::Graphics:
                queueFamily = m_Device->GetVKGraphicsQueueFamily();
                break;
        }
        m_Queue = m_Device->GetVKLogicalDevice().getQueue(queueFamily, 0);

        vk::CommandPoolCreateInfo commandPoolCreateInfo;
        commandPoolCreateInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
        commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolCreateInfo.queueFamilyIndex = queueFamily;
        m_CommandPool = m_Device->GetVKLogicalDevice().createCommandPool(commandPoolCreateInfo);

        vk::SemaphoreTypeCreateInfo timelineSemaphoreCreateInfo;
        timelineSemaphoreCreateInfo.sType = vk::StructureType::eSemaphoreTypeCreateInfo;
        timelineSemaphoreCreateInfo.semaphoreType = vk::SemaphoreType::eTimeline;
        timelineSemaphoreCreateInfo.initialValue = 0;

        vk::SemaphoreCreateInfo semaphoreCreateInfo;
        semaphoreCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;
        semaphoreCreateInfo.pNext = &timelineSemaphoreCreateInfo;
        m_Semaphore = m_Device->GetVKLogicalDevice().createSemaphore(semaphoreCreateInfo);

        m_LastSyncPoint.Queue = this;
        m_LastSyncPoint.Value = 0;
    }

    VulkanQueue::~VulkanQueue()
    {
        m_Device->GetVKLogicalDevice().destroySemaphore(m_Semaphore);
        m_Device->GetVKLogicalDevice().destroyCommandPool(m_CommandPool);
    }

    Ref<CommandBuffer> VulkanQueue::CreateCommandBuffer()
    {
        return CreateRef<VulkanCommandBuffer>(m_Device, this);
    }

    Queue::SyncPoint VulkanQueue::SubmitCommandBuffer(Ref<CommandBuffer> commandBuffer, std::vector<SyncPoint> waitSyncPoints)
    {
        std::vector<vk::Semaphore> waitSemaphores;
        std::vector<uint64_t> waitValues;
        for (const auto& waitSyncPoint : waitSyncPoints)
        {
            VulkanQueue* waitQueue = static_cast<VulkanQueue*>(waitSyncPoint.Queue);
            vk::Semaphore waitSemaphore = waitQueue->GetVKTImelineSemaphore();
            waitSemaphores.push_back(waitSemaphore);
            waitValues.push_back(waitSyncPoint.Value);
        }
        waitSemaphores.push_back(m_Semaphore);
        waitValues.push_back(m_LastSyncPoint.Value);
        std::vector<vk::PipelineStageFlags> waitStages(waitSemaphores.size(), vk::PipelineStageFlagBits::eAllCommands);

        vk::CommandBuffer nativeCommandBuffer;
        if (commandBuffer)
        {
            Ref<VulkanCommandBuffer> vulkanCommandBuffer = std::dynamic_pointer_cast<VulkanCommandBuffer>(commandBuffer);
            nativeCommandBuffer = vulkanCommandBuffer->GetVKCommandBuffer();
        }

        SyncPoint newSyncPoint;
        newSyncPoint.Queue = this;
        newSyncPoint.Value = m_LastSyncPoint.Value + 1;

        vk::TimelineSemaphoreSubmitInfo timelineSemaphoreSubmitInfo;
        timelineSemaphoreSubmitInfo.sType = vk::StructureType::eTimelineSemaphoreSubmitInfo;
        timelineSemaphoreSubmitInfo.waitSemaphoreValueCount = waitValues.size();
        timelineSemaphoreSubmitInfo.pWaitSemaphoreValues = waitValues.data();
        timelineSemaphoreSubmitInfo.signalSemaphoreValueCount = 1;
        timelineSemaphoreSubmitInfo.pSignalSemaphoreValues = &newSyncPoint.Value;

        vk::SubmitInfo submitInfo;
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        submitInfo.pNext = &timelineSemaphoreSubmitInfo;
        submitInfo.waitSemaphoreCount = waitSemaphores.size();
        submitInfo.pWaitDstStageMask = waitStages.data();
        submitInfo.pWaitSemaphores = waitSemaphores.data();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_Semaphore;
        submitInfo.commandBufferCount = commandBuffer ? 1 : 0;
        submitInfo.pCommandBuffers = commandBuffer ? &nativeCommandBuffer : nullptr;

        vk::Result result = m_Queue.submit(1, &submitInfo, nullptr);
        if (result != vk::Result::eSuccess)
        {
            NB_CORE_ERROR("Failed to submit command buffer: {}", static_cast<int>(result));
        }

        m_LastSyncPoint = newSyncPoint;
        return newSyncPoint;
    }

    void VulkanQueue::NewFrame(vk::Semaphore imageAcquiredSemaphore)
    {
        std::vector<uint64_t> waitValues = { m_LastSyncPoint.Value, 0 };
        std::vector<vk::Semaphore> waitSemaphores = { m_Semaphore, imageAcquiredSemaphore };

        SyncPoint newSyncPoint;
        newSyncPoint.Queue = this;
        newSyncPoint.Value = m_LastSyncPoint.Value + 1;

        vk::TimelineSemaphoreSubmitInfo timelineSemaphoreSubmitInfo;
        timelineSemaphoreSubmitInfo.sType = vk::StructureType::eTimelineSemaphoreSubmitInfo;
        timelineSemaphoreSubmitInfo.waitSemaphoreValueCount = waitValues.size();
        timelineSemaphoreSubmitInfo.pWaitSemaphoreValues = waitValues.data();
        timelineSemaphoreSubmitInfo.signalSemaphoreValueCount = 1;
        timelineSemaphoreSubmitInfo.pSignalSemaphoreValues = &newSyncPoint.Value;

        vk::PipelineStageFlags waitStage[] = { vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands };

        vk::SubmitInfo submitInfo;
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        submitInfo.pNext = &timelineSemaphoreSubmitInfo;
        submitInfo.waitSemaphoreCount = waitSemaphores.size();
        submitInfo.pWaitDstStageMask = waitStage;
        submitInfo.pWaitSemaphores = waitSemaphores.data();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_Semaphore;

        vk::Result result = m_Queue.submit(1, &submitInfo, nullptr);
        if (result != vk::Result::eSuccess)
        {
            NB_CORE_ERROR("Failed to submit command buffer: {}", static_cast<int>(result));
        }

        m_LastSyncPoint = newSyncPoint;
    }

    void VulkanQueue::EndFrame(vk::Semaphore renderCompleteSemaphore, vk::Fence frameFence)
    {
        std::vector<uint64_t> signalValues = { m_LastSyncPoint.Value + 1, 0 };
        std::vector<vk::Semaphore> signalSemaphores = { m_Semaphore, renderCompleteSemaphore };

        SyncPoint newSyncPoint;
        newSyncPoint.Queue = this;
        newSyncPoint.Value = m_LastSyncPoint.Value + 1;

        vk::TimelineSemaphoreSubmitInfo timelineSemaphoreSubmitInfo;
        timelineSemaphoreSubmitInfo.sType = vk::StructureType::eTimelineSemaphoreSubmitInfo;
        timelineSemaphoreSubmitInfo.waitSemaphoreValueCount = 1;
        timelineSemaphoreSubmitInfo.pWaitSemaphoreValues = &m_LastSyncPoint.Value;
        timelineSemaphoreSubmitInfo.signalSemaphoreValueCount = signalValues.size();
        timelineSemaphoreSubmitInfo.pSignalSemaphoreValues = signalValues.data();

        vk::PipelineStageFlags waitStage[] = { vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands };

        vk::SubmitInfo submitInfo;
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        submitInfo.pNext = &timelineSemaphoreSubmitInfo;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitDstStageMask = waitStage;
        submitInfo.pWaitSemaphores = &m_Semaphore;
        submitInfo.signalSemaphoreCount = signalSemaphores.size();
        submitInfo.pSignalSemaphores = signalSemaphores.data();

        vk::Result result = m_Queue.submit(1, &submitInfo, frameFence);
        if (result != vk::Result::eSuccess)
        {
            NB_CORE_ERROR("Failed to submit command buffer: {}", static_cast<int>(result));
        }

        m_LastSyncPoint = newSyncPoint;
    }

    vk::QueueFlagBits VulkanQueue::GetVKQueueFlagBits(QueueType type)
    {
        switch (type)
        {
        case QueueType::Graphics: return vk::QueueFlagBits::eGraphics;
        }
    }
}
