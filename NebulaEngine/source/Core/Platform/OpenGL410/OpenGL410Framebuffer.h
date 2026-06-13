#pragma once

#include <cstdint>

#include "Core/Nebula/Renderer/Buffers/Framebuffer.h"

namespace Nebula {

    class OpenGL410Framebuffer : public Framebuffer
    {
    public:
        OpenGL410Framebuffer(const FramebufferSpecification& spec);
        virtual ~OpenGL410Framebuffer() override;

        void Invalidate();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual Ref<Texture2D> GetColorAttachment(uint32_t index) const override { return m_ColorTexture; }

        [[nodiscard]] virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

        virtual void Resize(uint32_t width, uint32_t height) override;

    private:
        uint32_t m_RendererID = 0;
        Ref<Texture2D> m_ColorTexture = 0;
        Ref<Texture2D> m_DepthTexture = 0;
        FramebufferSpecification m_Specification;

    };

}