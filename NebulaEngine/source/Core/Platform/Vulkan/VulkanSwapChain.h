#pragma once

#include "VulkanDevice.h"
#include "Platform/Nebula/CoreSystems/Graphics/Surface.h"
#include "Nebula/Graphics/Device.h"
#include "Nebula/Graphics/SwapChain.h"
#include "vulkan/vulkan.hpp"

namespace Nebula::Graphics
{
    class VulkanSwapChain : public SwapChain
    {
    public:
        VulkanSwapChain(Device* device, Ref<Surface> surface, const FramebufferSpecification& framebufferSpec, uint32_t swapchainTextureIndex);
        ~VulkanSwapChain() override;

        virtual void Resize(uint32_t width, uint32_t height) override;

        virtual void NewFrame() override;
        virtual Ref<Framebuffer> GetCurrentFrame() override;
        virtual void Present() override;

    public:
        void Invalidate();

    private:
        VulkanDevice* m_Device;
        Ref<Surface> m_Surface;
        vk::SurfaceKHR m_VKSurface;

        FramebufferSpecification m_FramebufferSpec;
        uint32_t m_SwapchainTextureIndex;

        vk::SwapchainKHR m_Swapchain;
        uint8_t m_MaxFramesInFlight;
        std::vector<Ref<Framebuffer>> m_Framebuffers;
    };
}
