#pragma once

#include <stdint.h>

#include "Platform/Nebula/CoreSystems/Rendering/Context.h"
#include "SwiftMetal3Context.h"

namespace Nebula::Platform
{

    class Metal3Context : public Context
    {
    public:
        Metal3Context(void* window);
        ~Metal3Context();

        virtual void Init() override;

        virtual void SetCurrent() override;
        virtual void SetVSync(VSyncMode mode) override;
        virtual VSyncMode IsVsync() const override;
        virtual void SwapBuffers() override;
        virtual void Present(uint32_t framebuffer) override;

    public:
        void* GetMetalLayer() { return m_MetalLayer; }
        void* GetMetalDevice() { return m_MetalDevice; }

    private:
        SwiftMetal3Context::Metal3Context m_NativeMetal3Context;

        void* m_MetalLayer;
        void* m_MetalDevice;
    };

}
