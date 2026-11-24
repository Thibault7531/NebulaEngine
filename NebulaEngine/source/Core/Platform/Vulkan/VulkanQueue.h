#pragma once

#include "VulkanDevice.h"
#include "Nebula/Graphics/Queue.h"
#include "Nebula/Graphics/Queue.h"

namespace Nebula::Graphics
{
    class VulkanQueue : public Queue
    {
    public:
        VulkanQueue(Device* device, QueueType type);
        ~VulkanQueue() override;

        virtual Ref<CommandBuffer> CreateCommandBuffer() override;
        virtual SyncPoint SubmitCommandBuffer(Ref<CommandBuffer> commandBuffer, std::vector<SyncPoint> waitSyncPoints) override;

        virtual SyncPoint GetLastSyncPoint() const override { return m_LastSyncPoint; }

    public:
        void NewFrame(vk::Semaphore imageAcquiredSemaphore);
        void EndFrame(vk::Semaphore renderCompleteSemaphore, vk::Fence frameFence);

        vk::Queue GetVKQueue() const { return m_Queue; }
        vk::CommandPool GetVKCommandPool() const { return m_CommandPool; }
        vk::Semaphore GetVKTImelineSemaphore() const { return m_Semaphore; }

    public:
        static vk::QueueFlagBits GetVKQueueFlagBits(QueueType type);

    private:
        VulkanDevice* m_Device;
        QueueType m_Type;

        vk::Queue m_Queue;
        vk::CommandPool m_CommandPool;
        vk::Semaphore m_Semaphore;
        SyncPoint m_LastSyncPoint;
    };
}
