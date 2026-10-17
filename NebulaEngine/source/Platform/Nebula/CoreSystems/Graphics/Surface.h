#pragma once

#include <string>

namespace Nebula::Graphics
{
    enum class VSyncMode
    {
        Off,
        On,
        Adaptive
    };

    class Surface
    {
    public:
        virtual ~Surface() = default;

        virtual std::string GetAPIVersion() const = 0;
        virtual std::string GetAPIVendor() const = 0;
        virtual std::string GetAPIRenderer() const = 0;

        virtual void SetInstance(void* instance) = 0;
        virtual void SetDevice(void* physicalDevice, void* logicalDevice, void* commandQueue) = 0;
        virtual void* GetSurface() const = 0;

        virtual void SetVSync(VSyncMode mode) = 0;
        [[nodiscard]] virtual VSyncMode IsVsync() const = 0;
        std::string GetVSyncModeString() const { return GetVSyncModeString(IsVsync()); }
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual void NewFrame() = 0;
        virtual void Present(void* texture) = 0;

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