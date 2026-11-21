#pragma once

#include "Metal3Device.h"
#include "Nebula/Graphics/Queue.h"

namespace Nebula::Graphics
{
    class Metal3Queue : public Queue
    {
    public:
        Metal3Queue(Device* device);
        ~Metal3Queue();

        virtual Ref<CommandBuffer> CreateCommandBuffer() override;
        virtual SyncPoint SubmitCommandBuffer(Ref<CommandBuffer> commandBuffer, std::vector<SyncPoint> waitSyncPoints) override;

        virtual SyncPoint GetLastSyncPoint() const override { return m_LastSyncPoint; }

    public:
        MTL::CommandQueue* GetMetalCommandQueue() const { return m_CommandQueue; }
        MTL::CommandBuffer* GetLastCommandBuffer() const { return m_LastCommandBuffer; }
        MTL::Event* GetTimelineEvent() const { return m_TimelineEvent; }

    private:
        Metal3Device* m_Device;
        MTL::CommandQueue* m_CommandQueue;
        MTL::SharedEvent* m_TimelineEvent;

        MTL::CommandBuffer* m_LastCommandBuffer;
        SyncPoint m_LastSyncPoint;
    };
}
