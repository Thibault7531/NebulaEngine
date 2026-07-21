#pragma once

#include "Metal3Texture.h"
#include "Metal/Metal.hpp"
#include "Nebula/Graphics/Device.h"
#include "Nebula/Graphics/Buffers/Framebuffer.h"

namespace Nebula::Graphics
{
    class Metal3Framebuffer : public Framebuffer
    {
    public:
        Metal3Framebuffer(Device* device, const FramebufferSpecification& spec);
        ~Metal3Framebuffer();

        virtual void SetClearColors(const std::vector<glm::vec4>& colors) override { m_ClearColors = colors; }
        virtual void SetClearDepth(float depth) override { m_ClearDepth = depth; }
        virtual void SetClearStencil(int32_t stencil) override { m_ClearStencil = stencil; }

        virtual Ref<Texture2D> GetColorAttachment(uint32_t index) const override { return m_ColorAttachments[index]; }
        virtual const std::vector<Texture::TextureSpecification>& GetColorAttachmentSpecs() const override { return m_ColorAttachmentSpecs; }
        virtual const std::vector<glm::vec4>& GetClearColors() const override { return m_ClearColors; }
        virtual Ref<Texture2D> GetDepthAttachment() const override {return m_DepthAttachment; }
        virtual const Texture::TextureSpecification& GetDepthAttachmentSpec() const override { return m_DepthAttachmentSpec; }
        virtual float GetClearDepth() const override { return m_ClearDepth; }

        virtual const FramebufferSpecification& GetSpecification() const override { return m_Spec; }
        virtual void* GetNativeFramebuffer() override { return m_ColorAttachments[0]->GetMetalTexture(); }

        virtual void Resize(uint32_t width, uint32_t height) override;

    private:
        void Invalidate();

    private:
        std::vector<Texture::TextureSpecification> m_ColorAttachmentSpecs;
        std::vector<Ref<Metal3Texture2D>> m_ColorAttachments;
        std::vector<glm::vec4> m_ClearColors{{0.f, 0.f, 0.f, 1.f}};

        Texture::TextureSpecification m_DepthAttachmentSpec{Texture::InternalFormat::None};
        Ref<Metal3Texture2D> m_DepthAttachment;
        float m_ClearDepth = 1.f;
        int32_t m_ClearStencil = 1;

        FramebufferSpecification m_Spec;
        Device* m_Device;
    };
}