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

    void VulkanSwapChain::NewFrame() {}
    Ref<Framebuffer> VulkanSwapChain::GetCurrentFrame() { return nullptr; }
    void VulkanSwapChain::Present() {}

    void VulkanSwapChain::Invalidate()
    {
        if (m_Swapchain)
        {
            m_Device->GetVKLogicalDevice().waitIdle();
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

        if (swapchainCreateInfo.oldSwapchain)
        {
            m_Device->GetVKLogicalDevice().destroySwapchainKHR(swapchainCreateInfo.oldSwapchain);
        }
    }
}
