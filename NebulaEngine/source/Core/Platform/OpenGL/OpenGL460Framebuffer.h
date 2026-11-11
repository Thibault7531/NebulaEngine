#pragma once

#include "OpenGL460Texture.h"
#include "Nebula/Graphics/Device.h"
#include "Nebula/Graphics/Texture.h"
#include "Nebula/Graphics/Buffers/Framebuffer.h"

namespace Nebula::Graphics {

    class OpenGL460Framebuffer : public Framebuffer
    {
    public:
        OpenGL460Framebuffer(const FramebufferSpecification& spec);
        virtual ~OpenGL460Framebuffer();

        void Invalidate();

        virtual void SetClearColors(const std::vector<glm::vec4>& colors) override { m_ClearColors = colors; }
        virtual void SetClearDepth(float depth) override { m_ClearDepth = depth; }
        virtual void SetClearStencil(int32_t stencil) override { m_ClearStencil = stencil; }

        virtual uint32_t GetColorAttachmentCount() const override { return m_ColorAttachments.size(); }
        virtual Ref<Texture2D> GetColorAttachment(uint32_t index) const override { return std::dynamic_pointer_cast<Texture2D>(m_ColorAttachments[index]); }
        virtual Ref<Texture2D> GetSwapchainColorAttachment() const override { return GetColorAttachment(0); }
        virtual const std::vector<Texture::TextureSpecification>& GetColorAttachmentSpecs() const override { return m_ColorAttachmentSpecs; }
        virtual const std::vector<glm::vec4>& GetClearColors() const override { return m_ClearColors; }
        virtual Ref<Texture2D> GetDepthAttachment() const override { return std::dynamic_pointer_cast<Texture2D>(m_DepthAttachment); }
        virtual const Texture::TextureSpecification& GetDepthAttachmentSpec() const override { return m_DepthAttachmentSpec; }
        virtual float GetClearDepth() const override { return m_ClearDepth; }

        [[nodiscard]] virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

        virtual void Resize(uint32_t width, uint32_t height) override;

    public:
        GLuint GetBufferID() const { return m_RendererID; }
        void Clear();

    private:
        static ColorAttachementDataType GetColorAttachmentDataType(Texture::InternalFormat format);

    private:
        uint32_t m_RendererID = 0;

        std::vector<Texture::TextureSpecification> m_ColorAttachmentSpecs;
        Texture::TextureSpecification m_DepthAttachmentSpec{Texture::InternalFormat::None};

        std::vector<Ref<OpenGL460Texture2D>> m_ColorAttachments{};
        Ref<OpenGL460Texture2D> m_DepthAttachment;

        std::vector<glm::vec4> m_ClearColors = { { 0.f, 0.f, 0.f, 1.0f } };
        float m_ClearDepth = 1.0f;
        int32_t m_ClearStencil = 1;

        FramebufferSpecification m_Specification;

    };

}