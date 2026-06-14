#pragma once

#include "Nebula/Renderer/Texture.h"
#include "Nebula/Renderer/Buffers/Framebuffer.h"

namespace Nebula {

    class OpenGL460Framebuffer : public Framebuffer
    {
    public:
        OpenGL460Framebuffer(const FramebufferSpecification& spec);
        virtual ~OpenGL460Framebuffer() override;

        void Invalidate();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual Ref<Texture2D> GetColorAttachment(uint32_t index) const override { return m_ColorAttachments[index]; }

        [[nodiscard]] virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

        virtual void Resize(uint32_t width, uint32_t height) override;

    private:
        uint32_t m_RendererID = 0;
        std::vector<Ref<Texture2D>> m_ColorAttachments{};
        Ref<Texture2D> m_DepthTexture = 0;
        FramebufferSpecification m_Specification;

    };

}