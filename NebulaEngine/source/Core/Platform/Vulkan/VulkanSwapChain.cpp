#include "VulkanSwapChain.h"

#include "VulkanQueue.h"
#include "VulkanTexture.h"

namespace Nebula::Graphics
{
    VulkanSwapChain::VulkanSwapChain(Device* device, Ref<Surface> surface, const FramebufferSpecification& framebufferSpec,
        uint32_t swapchainTextureIndex)
            : m_Device{dynamic_cast<VulkanDevice*>(device)}, m_Surface{surface}, m_FramebufferSpec{framebufferSpec}, m_SwapchainTextureIndex{swapchainTextureIndex}
    {
        m_Device->SetSurfaceDevice(m_Surface);
        m_VKSurface = static_cast<VkSurfaceKHR>(m_Surface->GetSurface());
        Invalidate();
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        m_Device->GetVKLogicalDevice().waitIdle();
        m_Device->GetVKLogicalDevice().destroySwapchainKHR(m_Swapchain);
    }

    void VulkanSwapChain::Resize(uint32_t width, uint32_t height)
    {
        m_FramebufferSpec.width = width;
        m_FramebufferSpec.height = height;
        Invalidate();
    }

    void VulkanSwapChain::NewFrame()
    {
        vk::Result result = m_Device->GetVKLogicalDevice().waitForFences(1, &m_FrameSlots[m_CurrentFrameSlotIndex].Fence, true, UINT64_MAX);
        if (result != vk::Result::eSuccess)
        {
            NB_CORE_ERROR("Failed to wait for fence: {}", static_cast<int>(result));
            return;
        }

        result = m_Device->GetVKLogicalDevice().acquireNextImageKHR(m_Swapchain, UINT64_MAX, m_FrameSlots[m_CurrentFrameSlotIndex].ImageAcquiredSemaphore, nullptr, &m_CurrentFrameIndex);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
        {
            Invalidate();
            NewFrame();
            return;
        }
        if (result != vk::Result::eSuccess)
        {
            NB_CORE_ERROR("Failed to acquire next image from swapchain: {}", static_cast<int>(result));
            return;
        }

        result = m_Device->GetVKLogicalDevice().resetFences(1, &m_FrameSlots[m_CurrentFrameSlotIndex].Fence);
        if (result != vk::Result::eSuccess)
        {
            NB_CORE_ERROR("Failed to reset fence: {}", static_cast<int>(result));
            return;
        }

        Ref<VulkanQueue> graphicsQueue = std::dynamic_pointer_cast<VulkanQueue>(m_Device->GetGraphicsQueue());
        graphicsQueue->NewFrame(m_FrameSlots[m_CurrentFrameSlotIndex].ImageAcquiredSemaphore);
    }

    Ref<Framebuffer> VulkanSwapChain::GetCurrentFrame() { return m_Framebuffers[m_CurrentFrameIndex]; }

    void VulkanSwapChain::Present()
    {
        Ref<VulkanQueue> graphicsQueue = std::dynamic_pointer_cast<VulkanQueue>(m_Device->GetGraphicsQueue());
        graphicsQueue->EndFrame(m_FrameSlots[m_CurrentFrameSlotIndex].RenderCompleteSemaphore, m_FrameSlots[m_CurrentFrameSlotIndex].Fence);

        vk::PresentInfoKHR presentInfo;
        presentInfo.sType = vk::StructureType::ePresentInfoKHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_FrameSlots[m_CurrentFrameSlotIndex].RenderCompleteSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &m_CurrentFrameSlotIndex;

        vk::Result result = graphicsQueue->GetVKQueue().presentKHR(&presentInfo);
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
        {
            Invalidate();
            return;
        }
        if (result != vk::Result::eSuccess)
        {
            NB_CORE_ERROR("Failed to present swapchain image: {}", static_cast<int>(result));
            return;
        }

        m_CurrentFrameSlotIndex = (m_CurrentFrameSlotIndex + 1) % m_MaxFramesInFlight;
    }

    void VulkanSwapChain::Invalidate()
    {
        if (m_Swapchain)
        {
            m_Device->GetVKLogicalDevice().waitIdle();

            for (auto& slot : m_FrameSlots)
            {
                m_Device->GetVKLogicalDevice().destroyFence(slot.Fence);
                m_Device->GetVKLogicalDevice().destroySemaphore(slot.ImageAcquiredSemaphore);
                m_Device->GetVKLogicalDevice().destroySemaphore(slot.RenderCompleteSemaphore);
            }
            for (auto& framebuffer : m_Framebuffers)
            {
                framebuffer.reset();
            }
        }

        vk::Extent2D swapchainExtent;
        swapchainExtent.width = m_FramebufferSpec.width;
        swapchainExtent.height = m_FramebufferSpec.height;
        vk::Format textureFormat = VulkanTexture2D::GetVulkanInternalFormat(m_FramebufferSpec.AttachmentSpec.Attachments[m_SwapchainTextureIndex].InternalFormat);

        vk::SurfaceCapabilitiesKHR surfaceCapabilities = m_Device->GetVKPhysicalDevice().getSurfaceCapabilitiesKHR(m_VKSurface);

        vk::SwapchainCreateInfoKHR swapchainCreateInfo;
        swapchainCreateInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
        swapchainCreateInfo.surface = m_VKSurface;
        swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount;
        swapchainCreateInfo.imageFormat = textureFormat;
        swapchainCreateInfo.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
        swapchainCreateInfo.imageExtent = swapchainExtent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        swapchainCreateInfo.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
        swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        swapchainCreateInfo.presentMode = vk::PresentModeKHR::eFifo;
        swapchainCreateInfo.oldSwapchain = m_Swapchain;
        m_Swapchain = m_Device->GetVKLogicalDevice().createSwapchainKHR(swapchainCreateInfo);

        vk::FenceCreateInfo fenceCreateInfo;
        fenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo;
        fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

        vk::SemaphoreCreateInfo semaphoreCreateInfo;
        semaphoreCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;

        std::vector<vk::Image> swapchainImages = m_Device->GetVKLogicalDevice().getSwapchainImagesKHR(m_Swapchain);
        m_MaxFramesInFlight = std::min(3, static_cast<int32_t>(swapchainImages.size()));
        m_FrameSlots = std::vector<VulkanFrameSlot>(m_MaxFramesInFlight);
        m_Framebuffers = std::vector<Ref<Framebuffer>>(swapchainImages.size());
        for (uint32_t index = 0; index < swapchainImages.size(); index++)
        {
            Ref<Texture2D> texture = CreateRef<VulkanTexture2D>(m_Device, swapchainExtent.width, swapchainExtent.height, swapchainImages[index], m_FramebufferSpec.AttachmentSpec.Attachments[m_SwapchainTextureIndex]);
            m_Framebuffers[index] = m_Device->CreateFramebufferForSwapchain(m_FramebufferSpec, m_SwapchainTextureIndex, texture);
            m_FrameSlots[index].Fence = m_Device->GetVKLogicalDevice().createFence(fenceCreateInfo);
            m_FrameSlots[index].ImageAcquiredSemaphore = m_Device->GetVKLogicalDevice().createSemaphore(semaphoreCreateInfo);
            m_FrameSlots[index].RenderCompleteSemaphore = m_Device->GetVKLogicalDevice().createSemaphore(semaphoreCreateInfo);
        }
        m_CurrentFrameSlotIndex = 0;

        if (swapchainCreateInfo.oldSwapchain)
        {
            m_Device->GetVKLogicalDevice().destroySwapchainKHR(swapchainCreateInfo.oldSwapchain);
        }
    }
}
