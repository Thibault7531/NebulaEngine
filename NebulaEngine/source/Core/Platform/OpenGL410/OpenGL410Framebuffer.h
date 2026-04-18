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

        [[nodiscard]] virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }

        [[nodiscard]] virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

        virtual void Resize(uint32_t width, uint32_t height) override;

    private:
        uint32_t m_RendererID = 0;
        uint32_t m_ColorAttachment = 0;
        uint32_t m_DepthAttachment = 0;
        FramebufferSpecification m_Specification;

    };

}