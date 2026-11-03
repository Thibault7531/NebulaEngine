#include "VulkanFence.h"

namespace Nebula::Graphics
{
    VulkanFence::VulkanFence(Device* device, bool signaled)
        : m_Device(dynamic_cast<VulkanDevice*>(device))
    {
        vk::FenceCreateInfo fenceCreateInfo;
        fenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo;
        if (signaled) fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;
        m_Fence = m_Device->GetVKLogicalDevice().createFence(fenceCreateInfo);
    }

    VulkanFence::~VulkanFence()
    {
        m_Device->GetVKLogicalDevice().destroyFence(m_Fence);
    }

    void VulkanFence::Wait()
    {
        vk::Result result = m_Device->GetVKLogicalDevice().waitForFences(1, &m_Fence, true, 0);
        if (result != vk::Result::eSuccess)
        {
            NB_CORE_ERROR("Failed to wait for fence: {}", static_cast<int>(result));
        }
    }

    void VulkanFence::Reset()
    {
        vk::Result result = m_Device->GetVKLogicalDevice().resetFences(1, &m_Fence);
        if (result != vk::Result::eSuccess)
        {
            NB_CORE_ERROR("Failed to reset fence: {}", static_cast<int>(result));
        }
    }
}
