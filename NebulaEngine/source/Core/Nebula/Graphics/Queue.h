#pragma once

#include "CommandBuffer.h"
#include "Nebula/CoreSystems/Core/Core.h"

namespace Nebula::Graphics
{
    class Queue
    {
    public:
        enum class QueueType
        {
            Graphics
        };

        struct SyncPoint
        {
            Queue* Queue;
            uint64_t Value;
        };

    public:
        virtual ~Queue() = default;

        virtual Ref<CommandBuffer> CreateCommandBuffer() = 0;
        virtual SyncPoint SubmitCommandBuffer(Ref<CommandBuffer> commandBuffer, std::vector<SyncPoint> waitSyncPoints = {}) = 0;

        virtual SyncPoint GetLastSyncPoint() const = 0;
    };
}
