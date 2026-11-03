#pragma once

#include "VulkanDevice.h"
#include "Nebula/Graphics/Fence.h"

namespace Nebula::Graphics
{
    class VulkanFence : public Fence
    {
    public:
        VulkanFence(Device* device, bool signaled = false);
        ~VulkanFence() override;

        void Wait() override;
        virtual void Reset() override;

    public:
        vk::Fence GetVulkanFence() const { return m_Fence; }

    private:
        VulkanDevice* m_Device;
        vk::Fence m_Fence;
    };
}
