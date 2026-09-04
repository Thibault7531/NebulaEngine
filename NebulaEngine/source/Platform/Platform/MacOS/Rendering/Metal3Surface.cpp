#include "Metal3Surface.h"

#include <QuartzCore/QuartzCore.hpp>
#include <Metal/Metal.hpp>

namespace Nebula::Graphics
{

    Metal3Surface::Metal3Surface(void* window)
        : m_NativeMetal3Surface{SwiftMetal3Surface::CreateMetal3Surface(window)}, m_MetalLayer{m_NativeMetal3Surface.GetMetalLayer()}
    {
    }

    Metal3Surface::~Metal3Surface()
    {
    }

    void Metal3Surface::SetDevice(void* device, void* commandQueue)
    {
        m_NativeMetal3Surface.SetDevice(device);
        m_MetalDevice = device;
        m_MetalCommandQueue = static_cast<MTL::CommandQueue*>(commandQueue);
    }

    std::string Metal3Surface::GetAPIVersion() const
    {
        return "Metal 3";
    }

    std::string Metal3Surface::GetAPIVendor() const
    {
        MTL::GPUFamily families[] = {
            MTL::GPUFamilyApple10,
            MTL::GPUFamilyApple9,
            MTL::GPUFamilyApple8,
            MTL::GPUFamilyApple7,
            MTL::GPUFamilyApple6,
            MTL::GPUFamilyApple5,
            MTL::GPUFamilyApple4,
            MTL::GPUFamilyApple3,
            MTL::GPUFamilyApple2,
            MTL::GPUFamilyApple1,
            MTL::GPUFamilyMetal3,
            MTL::GPUFamilyMac2,
            MTL::GPUFamilyMac1,
            MTL::GPUFamilyCommon3,
            MTL::GPUFamilyCommon2,
            MTL::GPUFamilyCommon1,
            MTL::GPUFamilyMacCatalyst1,
            MTL::GPUFamilyMacCatalyst2,
            MTL::GPUFamilyMetal3,
            MTL::GPUFamilyMetal4
        };

        std::string gpuFamily = "Unknown";

        for (auto family : families)
        {
            if (static_cast<MTL::Device*>(m_MetalDevice)->supportsFamily(family))
            {
                gpuFamily = GPUFamilyToString(reinterpret_cast<void*>(family));
            }
        }
        return gpuFamily;
    }

    std::string Metal3Surface::GetAPIRenderer() const
    {
        return static_cast<MTL::Device*>(m_MetalDevice)->name()->utf8String();
    }

    void Metal3Surface::SetVSync(VSyncMode mode)
    {
        switch (mode)
        {
        case VSyncMode::Off:
            {
                static_cast<CA::MetalLayer*>(m_MetalLayer)->setDisplaySyncEnabled(false);
                return;
            }

        case VSyncMode::Adaptive:
            {
                static_cast<CA::MetalLayer*>(m_MetalLayer)->setDisplaySyncEnabled(true);
                return;
            }

        case VSyncMode::On:
            {
                static_cast<CA::MetalLayer*>(m_MetalLayer)->setDisplaySyncEnabled(true);
                return;
            }
        }
    }

    VSyncMode Metal3Surface::IsVsync() const
    {
        if (static_cast<CA::MetalLayer*>(m_MetalLayer)->displaySyncEnabled()) return VSyncMode::On;
        return VSyncMode::Off;
    }

    void Metal3Surface::Resize(uint32_t width, uint32_t height)
    {
        static_cast<CA::MetalLayer*>(m_MetalLayer)->setDrawableSize(CGSize(width, height));
    }

    void Metal3Surface::NewFrame()
    {
        m_MetalDrawable = static_cast<CA::MetalLayer*>(m_MetalLayer)->nextDrawable();
    }

    void Metal3Surface::Present(void* framebuffer)
    {
        CA::MetalDrawable* drawable = static_cast<CA::MetalDrawable*>(m_MetalDrawable);
        MTL::CommandBuffer* cmdBuffer = static_cast<MTL::CommandQueue*>(m_MetalCommandQueue)->commandBuffer();
        MTL::Texture* source = static_cast<MTL::Texture*>(framebuffer);

        MTL::BlitCommandEncoder* cmdEncoder = cmdBuffer->blitCommandEncoder();
        cmdEncoder->copyFromTexture(source, drawable->texture());
        cmdEncoder->endEncoding();

        cmdBuffer->presentDrawable(drawable);
        cmdBuffer->commit();
        cmdBuffer->waitUntilCompleted();
    }

    std::string Metal3Surface::GPUFamilyToString(void* gpuFamily)
    {
        switch (static_cast<MTL::GPUFamily>(reinterpret_cast<int64_t>(gpuFamily)))
        {
        case MTL::GPUFamilyApple1: return "Apple GPU Family 1 (Apple A7)";
        case MTL::GPUFamilyApple2: return "Apple GPU Family 2 (Apple A8/A8X)";
        case MTL::GPUFamilyApple3: return "Apple GPU Family 3 (Apple A9/A9X/A10/A10X)";
        case MTL::GPUFamilyApple4: return "Apple GPU Family 4 (Apple A11)";
        case MTL::GPUFamilyApple5: return "Apple GPU Family 5 (Apple A12/A12X/A12Z)";
        case MTL::GPUFamilyApple6: return "Apple GPU Family 6 (Apple A13)";
        case MTL::GPUFamilyApple7: return "Apple GPU Family 7 (Apple A14/M1)";
        case MTL::GPUFamilyApple8: return "Apple GPU Family 8 (Apple A15/A16/M2)";
        case MTL::GPUFamilyApple9: return "Apple GPU Family 9 (Apple A17Pro/M3/M4)";
        case MTL::GPUFamilyApple10: return "Apple GPU Family 10";
        case MTL::GPUFamilyMac1: return "Mac GPU Family 1";
        case MTL::GPUFamilyMac2: return "Mac GPU Family 2";
        case MTL::GPUFamilyCommon1: return "Common GPU Family 1";
        case MTL::GPUFamilyCommon2: return "Common GPU Family 2";
        case MTL::GPUFamilyCommon3: return "Common GPU Family 3";
        case MTL::GPUFamilyMacCatalyst1: return "Mac Catalyst GPU Family 1";
        case MTL::GPUFamilyMacCatalyst2: return "Mac Catalyst GPU Family 2";
        case MTL::GPUFamilyMetal3: return "Metal 3 GPU Family";
        case MTL::GPUFamilyMetal4: return "Metal 4 GPU Family";
        }
    }
}
