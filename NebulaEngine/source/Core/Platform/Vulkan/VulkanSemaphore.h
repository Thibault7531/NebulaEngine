#pragma once

#include "VulkanDevice.h"
#include "Nebula/Graphics/Semaphore.h"

namespace Nebula::Graphics
{
    class VulkanSemaphore : public Semaphore
    {
    public:
        VulkanSemaphore(Device* device);
        ~VulkanSemaphore() override;

    public:
        vk::Semaphore GetVulkanSemaphore() const { return m_Semaphore; }

    private:
        VulkanDevice* m_Device;
        vk::Semaphore m_Semaphore;
    };
}
