#pragma once

#include "../Texture.h"
#include "Nebula/CoreSystems/Core/Core.h"

namespace Nebula {

    struct FramebufferSpecification
    {
        uint32_t width, height;
        uint32_t samples = 1;

        bool SwapChainTarget = false;
    };

    class Framebuffer
    {
    public:
        virtual ~Framebuffer() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        virtual Ref<Texture2D> GetColorAttachment(uint32_t index = 0) const = 0;

        [[nodiscard]] virtual const FramebufferSpecification& GetSpecification() const = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);

    };

}
