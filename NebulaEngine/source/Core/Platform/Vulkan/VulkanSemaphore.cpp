#include "VulkanSemaphore.h"

namespace Nebula::Graphics
{
    VulkanSemaphore::VulkanSemaphore(Device* device)
        : m_Device(dynamic_cast<VulkanDevice*>(device))
    {
        vk::SemaphoreCreateInfo semaphoreCreateInfo;
        semaphoreCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;
        m_Semaphore = m_Device->GetVKLogicalDevice().createSemaphore(semaphoreCreateInfo);
    }

    VulkanSemaphore::~VulkanSemaphore()
    {
        m_Device->GetVKLogicalDevice().destroySemaphore(m_Semaphore);
    }
}
