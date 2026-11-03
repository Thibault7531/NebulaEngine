#include "VulkanSwapChain.h"

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
        m_Fences[m_CurrentFrameIndex]->Wait();

        vk::Result result = m_Device->GetVKLogicalDevice().acquireNextImageKHR(m_Swapchain, 0, m_ImageAcquiredSemaphores[m_CurrentFrameIndex]->GetVulkanSemaphore(), nullptr, &m_CurrentFramebufferIndex);
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

        m_Fences[m_CurrentFrameIndex]->Reset();
    }

    Ref<Framebuffer> VulkanSwapChain::GetCurrentFrame() { return m_Framebuffers[m_CurrentFramebufferIndex]; }

    void VulkanSwapChain::Present(Ref<Semaphore> renderCompleteSemaphore)
    {
        vk::Semaphore renderCompleteVKSemaphore = nullptr;
        if (renderCompleteSemaphore)
        {
            renderCompleteVKSemaphore = std::dynamic_pointer_cast<VulkanSemaphore>(renderCompleteSemaphore)->GetVulkanSemaphore();
        }

        vk::PresentInfoKHR presentInfo;
        presentInfo.sType = vk::StructureType::ePresentInfoKHR;
        presentInfo.waitSemaphoreCount = renderCompleteVKSemaphore ? 1 : 0;
        presentInfo.pWaitSemaphores = renderCompleteVKSemaphore ? &renderCompleteVKSemaphore : nullptr;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_Swapchain;
        presentInfo.pImageIndices = &m_CurrentFramebufferIndex;

        vk::Result result = m_Device->GetVKGraphicsQueue().presentKHR(&presentInfo);
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

        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_MaxFramesInFlight;
    }

    void VulkanSwapChain::Invalidate()
    {
        if (m_Swapchain)
        {
            m_Device->GetVKLogicalDevice().waitIdle();

            for (auto& semaphore : m_ImageAcquiredSemaphores)
            {
                semaphore.reset();
            }

            for (auto& fence : m_Fences)
            {
                fence.reset();
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

        m_MaxFramesInFlight = surfaceCapabilities.maxImageCount == 0 ? 3 : surfaceCapabilities.maxImageCount;
        m_Framebuffers = std::vector<Ref<Framebuffer>>(m_MaxFramesInFlight);
        std::vector<vk::Image> swapchainImages = m_Device->GetVKLogicalDevice().getSwapchainImagesKHR(m_Swapchain);
        for (uint32_t index = 0; index < swapchainImages.size(); index++)
        {
            Ref<Texture2D> texture = CreateRef<VulkanTexture2D>(m_Device, swapchainExtent.width, swapchainExtent.height, swapchainImages[index], m_FramebufferSpec.AttachmentSpec.Attachments[m_SwapchainTextureIndex]);
            m_Framebuffers[index] = m_Device->CreateFramebufferForSwapchain(m_FramebufferSpec, m_SwapchainTextureIndex, texture);
        }
        m_Fences = std::vector<Ref<VulkanFence>>(m_MaxFramesInFlight);
        m_ImageAcquiredSemaphores = std::vector<Ref<VulkanSemaphore>>(m_MaxFramesInFlight);
        for (uint32_t index = 0; index < m_MaxFramesInFlight; index++)
        {
            m_Fences[index] = std::dynamic_pointer_cast<VulkanFence>(m_Device->CreateFence(true));
            m_ImageAcquiredSemaphores[index] = std::dynamic_pointer_cast<VulkanSemaphore>(m_Device->CreateSemaphore());
        }
        m_CurrentFrameIndex = 0;

        if (swapchainCreateInfo.oldSwapchain)
        {
            m_Device->GetVKLogicalDevice().destroySwapchainKHR(swapchainCreateInfo.oldSwapchain);
        }
    }
}
