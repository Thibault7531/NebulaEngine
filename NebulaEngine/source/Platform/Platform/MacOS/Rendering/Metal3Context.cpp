#include "Metal3Context.h"

#include <QuartzCore/QuartzCore.hpp>
#include "Metal/Metal.hpp"

namespace Nebula::Platform
{

    Metal3Context::Metal3Context(void* window)
        : m_NativeMetal3Context{SwiftMetal3Context::CreateMetal3Context(window)}, m_MetalLayer{m_NativeMetal3Context.GetMetalLayer()}, m_MetalDevice{m_NativeMetal3Context.GetMetalDevice()}
    {
    }

    Metal3Context::~Metal3Context()
    {
    }

    void Metal3Context::Init()
    {
    }

    void Metal3Context::SetCurrent()
    {
    }

    void Metal3Context::SetVSync(VSyncMode mode)
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

    VSyncMode Metal3Context::IsVsync() const
    {
        if (static_cast<CA::MetalLayer*>(m_MetalLayer)->displaySyncEnabled()) return VSyncMode::On;
        return VSyncMode::Off;
    }

    void Metal3Context::SwapBuffers()
    {
    }

    void Metal3Context::Present(uint32_t framebuffer)
    {
    }
}
