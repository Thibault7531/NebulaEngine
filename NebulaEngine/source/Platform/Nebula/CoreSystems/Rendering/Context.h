#pragma once

namespace Nebula::Platform
{
    enum class VSyncMode
    {
        Off,
        On,
        Adaptive
    };

    class Context
    {
    public:
        virtual ~Context() = default;

        virtual void Init() = 0;

        virtual void SetCurrent() = 0;
        virtual void SetVSync(VSyncMode mode) = 0;
        [[nodiscard]] virtual VSyncMode IsVsync() const = 0;
        virtual void SwapBuffers() = 0;
    };

}