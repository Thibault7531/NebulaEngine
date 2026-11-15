#pragma once

#include <stdint.h>

#include "Platform/Nebula/CoreSystems/Graphics/Surface.h"
#include "SwiftMetal3Surface.h"
#include "Core/Nebula/CoreSystems/Core/Core.h"

namespace Nebula::Graphics
{

    class Metal3Surface : public Surface
    {
    public:
        Metal3Surface(void* window);
        ~Metal3Surface();

        virtual void SetInstance(void* instance) override {}
        virtual void SetDevice(void* physicalDevice, void* logicalDevice, void* commandQueue) override;
        virtual void* GetSurface() const override;

        virtual std::string GetAPIVersion() const override;
        virtual std::string GetAPIVendor() const override;
        virtual std::string GetAPIRenderer() const override;

    public:
        void* GetMetalLayer() { return m_MetalLayer; }

    public:
        static std::string GPUFamilyToString(void* gpuFamily);

    private:
        SwiftMetal3Surface::Metal3Surface m_NativeMetal3Surface;

        void* m_MetalLayer = nullptr;
        void* m_MetalDevice = nullptr;
        void* m_MetalCommandQueue = nullptr;

        void* m_MetalDrawable = nullptr;
    };

    class Metal3SurfacePointer
    {
    public:
        Metal3SurfacePointer(void* window) { m_Pointer = CreateRef<Metal3Surface>(window); }
        ~Metal3SurfacePointer() = default;

        Ref<Metal3Surface> GetSharedPointer() { return m_Pointer; }
        void* GetPointer() { return m_Pointer.get(); }

        virtual void SetDevice(void* device, void* commandQueue) { m_Pointer->SetDevice(nullptr, device, commandQueue); }

        virtual void SetVSync(VSyncMode mode) { m_Pointer->SetVSync(mode); }
        virtual VSyncMode IsVsync() const { return m_Pointer->IsVsync(); }

        virtual void NewFrame() { m_Pointer->NewFrame(); }
        virtual void Present(void* framebuffer) { m_Pointer->Present(framebuffer); }

    private:
        Ref<Metal3Surface> m_Pointer;
    };

}
