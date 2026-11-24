#include "Metal3Queue.h"

#include "Metal3CommandBuffer.h"

namespace Nebula::Graphics
{
    Metal3Queue::Metal3Queue(Device* device)
        : m_Device{dynamic_cast<Metal3Device*>(device)}, m_CommandQueue{m_Device->GetMetalDevice()->newCommandQueue()}
    {
        m_TimelineEvent = m_Device->GetMetalDevice()->newSharedEvent();

        m_LastSyncPoint.Queue = this;
        m_LastSyncPoint.Value = 0;
    }

    Metal3Queue::~Metal3Queue()
    {
        m_CommandQueue->release();
    }

    Ref<CommandBuffer> Metal3Queue::CreateCommandBuffer()
    {
        Ref<CommandBuffer> commandBuffer = CreateRef<Metal3CommandBuffer>(m_Device->GetMetalDevice(), m_CommandQueue);
        return commandBuffer;
    }

    Queue::SyncPoint Metal3Queue::SubmitCommandBuffer(Ref<CommandBuffer> commandBuffer, std::vector<SyncPoint> waitSyncPoints)
    {
        MTL::CommandBuffer* waitCommandBuffer = m_CommandQueue->commandBuffer();
        waitCommandBuffer->encodeWait(m_TimelineEvent, m_LastSyncPoint.Value);

        for (auto& syncPoint : waitSyncPoints)
        {
            Metal3Queue* queue = static_cast<Metal3Queue*>(syncPoint.Queue);
            waitCommandBuffer->encodeWait(queue->GetTimelineEvent(), syncPoint.Value);
        }

        waitCommandBuffer->commit();

        Ref<Metal3CommandBuffer> metalCommandBuffer = std::dynamic_pointer_cast<Metal3CommandBuffer>(commandBuffer);
        metalCommandBuffer->GetMetalCommandBuffer()->encodeSignalEvent(m_TimelineEvent, m_LastSyncPoint.Value + 1);
        metalCommandBuffer->GetMetalCommandBuffer()->commit();
        m_LastCommandBuffer = metalCommandBuffer->GetMetalCommandBuffer();

        SyncPoint syncPoint;
        syncPoint.Queue = this;
        syncPoint.Value = m_LastSyncPoint.Value + 1;
        m_LastSyncPoint = syncPoint;
        return m_LastSyncPoint;
    }
}
