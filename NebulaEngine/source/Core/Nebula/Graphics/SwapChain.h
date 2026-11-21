#pragma once

#include "Buffers/Framebuffer.h"
#include "Nebula/CoreSystems/Core/Core.h"

namespace Nebula::Graphics
{
    enum class VSyncMode
    {
        Off,
        On,
        Adaptive
    };

    class SwapChain
    {
    public:
        virtual ~SwapChain() = default;

        virtual void SetVSyncMode(VSyncMode mode) = 0;
        virtual VSyncMode GetVSyncMode() const = 0;
        std::string GetVSyncModeString() const { return GetVSyncModeString(GetVSyncMode()); }
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual void NewFrame() = 0;
        virtual Ref<Framebuffer> GetCurrentFrame() = 0;
        virtual void Present() = 0;

        virtual FramebufferSpecification GetFramebufferSpecification() = 0;
        virtual uint32_t GetMaxFramesInFlight() const = 0;
        virtual uint32_t GetCurrentFrameIndex() const = 0;

    public:
        inline static std::string GetVSyncModeString(VSyncMode mode)
        {
            switch (mode)
            {
            case VSyncMode::Off: return "Off";
            case VSyncMode::On: return "On";
            case VSyncMode::Adaptive: return "Adaptive";
            }
        }
    };
}
