#pragma once

#include <stdint.h>

#include "Platform/Nebula/CoreSystems/Graphics/Surface.h"
#include "SwiftMetal3Surface.h"

namespace Nebula::Graphics
{

    class Metal3Surface : public Surface
    {
    public:
        Metal3Surface(void* window);
        ~Metal3Surface();

        virtual void SetDevice(void* device, void* commandQueue) override;

        virtual void SetVSync(VSyncMode mode) override;
        virtual VSyncMode IsVsync() const override;

        virtual void NewFrame() override;
        virtual void Present(void* framebuffer) override;

    public:
        void* GetMetalLayer() { return m_MetalLayer; }

    private:
        SwiftMetal3Surface::Metal3Surface m_NativeMetal3Surface;

        void* m_MetalLayer = nullptr;
        void* m_MetalCommandQueue = nullptr;

        void* m_MetalDrawable = nullptr;
    };

}
