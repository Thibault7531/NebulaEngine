#pragma once

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

        virtual void SetDevice(void* device, void* commandQueue) = 0;

        virtual void SetVSync(VSyncMode mode) = 0;
        [[nodiscard]] virtual VSyncMode IsVsync() const = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual void NewFrame() = 0;
        virtual void Present(void* texture) = 0;
    };

}