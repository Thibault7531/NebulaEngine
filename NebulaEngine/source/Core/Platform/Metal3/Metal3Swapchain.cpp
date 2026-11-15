#include "Metal3Swapchain.h"

#include "Metal3Queue.h"
#include "QuartzCore/CAMetalLayer.hpp"

namespace Nebula::Graphics
{
    Metal3Swapchain::Metal3Swapchain(Device* device, Ref<Surface> surface, VSyncMode vsyncMode, const FramebufferSpecification& framebufferSpec, uint32_t swapchainTextureIndex)
        : m_Device{static_cast<Metal3Device*>(device)}, m_Surface{surface}, m_VSyncMode{vsyncMode}, m_FramebufferSpec{framebufferSpec}, m_SwapchainTextureIndex{swapchainTextureIndex}
    {
        m_MetalLayer = static_cast<CA::MetalLayer*>(surface->GetSurface());
        m_MetalLayer->setDisplaySyncEnabled(vsyncMode == VSyncMode::On || vsyncMode == VSyncMode::Adaptive);
        m_MaxFramesInFlight = std::max(m_MaxFramesInFlight, static_cast<uint32_t>(m_MetalLayer->maximumDrawableCount()));
        m_MetalDrawables.resize(m_MaxFramesInFlight);
        m_Framebuffers.resize(m_MaxFramesInFlight);
        m_InFlightFences.resize(m_MaxFramesInFlight);
        for (uint32_t i = 0; i < m_MaxFramesInFlight; i++)
        {
            m_InFlightFences[i] = m_Device->GetMetalDevice()->newSharedEvent();
        }
    }

    void Metal3Swapchain::SetVSyncMode(VSyncMode mode)
    {
        m_VSyncMode = mode;
        m_MetalLayer->setDisplaySyncEnabled(mode == VSyncMode::On || mode == VSyncMode::Adaptive);
    }

    void Metal3Swapchain::Resize(uint32_t width, uint32_t height)
    {
        m_FramebufferSpec.width = width;
        m_FramebufferSpec.height = height;
        m_MetalLayer->setDrawableSize(CGSize{static_cast<CGFloat>(width), static_cast<CGFloat>(height)});
    }

    void Metal3Swapchain::NewFrame()
    {
        m_InFlightFences[m_CurrentFrameIndex]->waitUntilSignaledValue(1, UINT64_MAX);

        m_MetalDrawables[m_CurrentFrameIndex] = m_MetalLayer->nextDrawable();
        MTL::Texture* texture = m_MetalDrawables[m_CurrentFrameIndex]->texture();
        Ref<Metal3Texture2D> nbTexture = CreateRef<Metal3Texture2D>(m_Device, texture->width(), texture->height(), texture, m_FramebufferSpec.AttachmentSpec.Attachments[m_SwapchainTextureIndex]);
        m_Framebuffers[m_CurrentFrameIndex] = std::dynamic_pointer_cast<Metal3Framebuffer>(m_Device->CreateFramebufferForSwapchain(m_FramebufferSpec, m_SwapchainTextureIndex, nbTexture));

        m_InFlightFences[m_CurrentFrameIndex]->setSignaledValue(0);
    }

    void Metal3Swapchain::Present()
    {
        Ref<Metal3Queue> queue = std::dynamic_pointer_cast<Metal3Queue>(m_Device->GetGraphicsQueue());
        MTL::CommandBuffer* commandBuffer = queue->GetMetalCommandQueue()->commandBuffer();
        commandBuffer->encodeWait(queue->GetTimelineEvent(), queue->GetLastSyncPoint().Value);

        commandBuffer->presentDrawable(m_MetalDrawables[m_CurrentFrameIndex]);
        commandBuffer->encodeSignalEvent(m_InFlightFences[m_CurrentFrameIndex], 1);
        commandBuffer->commit();

        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % m_MaxFramesInFlight;
    }
}
