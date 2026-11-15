#pragma once

#include "Metal3Device.h"
#include "Metal3Framebuffer.h"
#include "Nebula/Graphics/SwapChain.h"
#include "Platform/Nebula/CoreSystems/Graphics/Surface.h"
#include "QuartzCore/CAMetalDrawable.hpp"

namespace Nebula::Graphics
{
    class Metal3Swapchain : public SwapChain
    {
    public:
        Metal3Swapchain(Device* device, Ref<Surface> surface, VSyncMode vsyncMode, const FramebufferSpecification& framebufferSpec, uint32_t swapchainTextureIndex);
        ~Metal3Swapchain() override = default;

        virtual void SetVSyncMode(VSyncMode mode) override;
        virtual VSyncMode GetVSyncMode() const override { return m_VSyncMode; }
        virtual void Resize(uint32_t width, uint32_t height) override;

        virtual void NewFrame() override;
        virtual Ref<Framebuffer> GetCurrentFrame() override { return m_Framebuffers[m_CurrentFrameIndex]; }
        virtual void Present() override;

        virtual uint32_t GetMaxFramesInFlight() const override { return m_MaxFramesInFlight; }
        virtual uint32_t GetCurrentFrameIndex() const override { return m_CurrentFrameIndex; }

    private:
        Metal3Device* m_Device;
        Ref<Surface> m_Surface;
        VSyncMode m_VSyncMode;
        FramebufferSpecification m_FramebufferSpec;
        uint32_t m_SwapchainTextureIndex;

        CA::MetalLayer* m_MetalLayer;
        uint32_t m_MaxFramesInFlight = 3;
        uint32_t m_CurrentFrameIndex = 0;
        std::vector<CA::MetalDrawable*> m_MetalDrawables;
        std::vector<Ref<Metal3Framebuffer>> m_Framebuffers;
        std::vector<MTL::SharedEvent*> m_InFlightFences;
    };
}
