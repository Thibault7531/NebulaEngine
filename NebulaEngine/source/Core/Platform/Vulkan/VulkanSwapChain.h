#pragma once

#include "VulkanDevice.h"
#include "Platform/Nebula/CoreSystems/Graphics/Surface.h"
#include "Nebula/Graphics/Device.h"
#include "Nebula/Graphics/SwapChain.h"
#include "vulkan/vulkan.hpp"

namespace Nebula::Graphics
{
    struct VulkanFrameSlot
    {
        vk::Fence Fence;
        vk::Semaphore ImageAcquiredSemaphore;
    };

    struct VulkanFrame
    {
        Ref<Framebuffer> Framebuffer;
        vk::Semaphore RenderCompleteSemaphore;
    };

    class VulkanSwapChain : public SwapChain
    {
    public:
        VulkanSwapChain(Device* device, Ref<Surface> surface, VSyncMode vsyncMode, const FramebufferSpecification& framebufferSpec, uint32_t swapchainTextureIndex);
        ~VulkanSwapChain() override;

        virtual void SetVSyncMode(VSyncMode mode) override;
        virtual VSyncMode GetVSyncMode() const override { return m_VSyncMode; }
        virtual void Resize(uint32_t width, uint32_t height) override;

        virtual void NewFrame() override;
        virtual Ref<Framebuffer> GetCurrentFrame() override;
        virtual void Present() override;

        virtual FramebufferSpecification GetFramebufferSpecification() override { return m_FramebufferSpec; }
        virtual uint32_t GetMaxFramesInFlight() const override { return m_MaxFramesInFlight; }
        virtual uint32_t GetCurrentFrameIndex() const override { return m_CurrentFrameSlotIndex; }

    public:
        void Invalidate();

    public:
        vk::PresentModeKHR GetVKPresentMode(VSyncMode mode);

    private:
        VulkanDevice* m_Device;
        Ref<Surface> m_Surface;
        vk::SurfaceKHR m_VKSurface;

        FramebufferSpecification m_FramebufferSpec;
        uint32_t m_SwapchainTextureIndex;

        vk::SwapchainKHR m_Swapchain;
        VSyncMode m_VSyncMode;
        uint8_t m_MaxFramesInFlight;
        uint32_t m_CurrentFrameSlotIndex;
        std::vector<VulkanFrameSlot> m_FrameSlots;
        uint32_t m_CurrentFrameIndex = 0;
        std::vector<VulkanFrame> m_Frames;
    };
}
