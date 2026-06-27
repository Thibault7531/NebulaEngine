#pragma once

#include "Nebula/Renderer/Texture.h"
#include "Nebula/Renderer/Buffers/Framebuffer.h"

namespace Nebula {

    class OpenGL460Framebuffer : public Framebuffer
    {
    public:
        OpenGL460Framebuffer(const FramebufferSpecification& spec);
        virtual ~OpenGL460Framebuffer();

        void Invalidate();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void SetClearColors(const std::vector<glm::vec4>& colors) override { m_ClearColors = colors; }
        virtual void SetClearDepth(float depth) override { m_ClearDepth = depth; }
        virtual void SetClearStencil(int32_t stencil) override { m_ClearStencil = stencil; }
        virtual void Clear() override;

        virtual Ref<Texture2D> GetColorAttachment(uint32_t index) const override { return m_ColorAttachments[index]; }

        [[nodiscard]] virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

        virtual void Resize(uint32_t width, uint32_t height) override;

    private:
        static ColorAttachementDataType GetColorAttachmentDataType(Texture::InternalFormat format);

    private:
        uint32_t m_RendererID = 0;

        std::vector<Texture::TextureSpecification> m_ColorAttachmentSpecs;
        Texture::TextureSpecification m_DepthAttachmentSpec{Texture::InternalFormat::None};

        std::vector<Ref<Texture2D>> m_ColorAttachments{};
        Ref<Texture2D> m_DepthAttachment;

        std::vector<glm::vec4> m_ClearColors = { { 0.f, 0.f, 0.f, 1.0f } };
        float m_ClearDepth = 1.0f;
        int32_t m_ClearStencil = 1;

        FramebufferSpecification m_Specification;

    };

}