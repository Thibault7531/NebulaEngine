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
        m_MetalCommandQueue = static_cast<MTL::CommandQueue*>(commandQueue);
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
}
