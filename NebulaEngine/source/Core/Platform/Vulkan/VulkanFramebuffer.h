#pragma once

#include "vk_mem_alloc.hpp"
#include "VulkanTexture.h"
#include "Nebula/Graphics/Device.h"
#include "Nebula/Graphics/Buffers/Framebuffer.h"

namespace Nebula::Graphics
{
    class VulkanFramebuffer : public Framebuffer
    {
    public:
        VulkanFramebuffer(Device* device, FramebufferSpecification framebufferSpec);
        VulkanFramebuffer(Device* device, FramebufferSpecification framebufferSpec, uint32_t swapchainTextureIndex, Ref<Texture2D> swapchainTexture);
        virtual ~VulkanFramebuffer();

        virtual void SetClearColors(const std::vector<glm::vec4>& colors) override { m_ClearColors = colors; }
        virtual void SetClearDepth(float depth) override { m_ClearDepth = depth; }
        virtual void SetClearStencil(int32_t stencil) override { m_ClearStencil = stencil; }

        virtual Ref<Texture2D> GetColorAttachment(uint32_t index) const override { return m_ColorAttachments[index]; }
        virtual const std::vector<Texture::TextureSpecification>& GetColorAttachmentSpecs() const override { return m_ColorAttachmentSpecs; }
        virtual const std::vector<glm::vec4>& GetClearColors() const override { return m_ClearColors; }
        virtual Ref<Texture2D> GetDepthAttachment() const override { return m_DepthAttachment; }
        virtual const Texture::TextureSpecification& GetDepthAttachmentSpec() const override { return m_DepthAttachmentSpec; }
        virtual float GetClearDepth() const override { return m_ClearDepth; }

        virtual const FramebufferSpecification& GetSpecification() const override { return m_Spec; }

        virtual void Resize(uint32_t width, uint32_t height) override;

    public:
        void Invalidate();

    private:
        std::vector<Texture::TextureSpecification> m_ColorAttachmentSpecs;
        std::vector<Ref<VulkanTexture2D>> m_ColorAttachments;
        std::vector<glm::vec4> m_ClearColors{{0.f, 0.f, 0.f, 1.f}};

        Texture::TextureSpecification m_DepthAttachmentSpec{Texture::InternalFormat::None};
        Ref<VulkanTexture2D> m_DepthAttachment;
        float m_ClearDepth = 1.f;
        int32_t m_ClearStencil = 1;

        FramebufferSpecification m_Spec;
        uint32_t m_SwapchainTextureIndex;
        Ref<VulkanTexture2D> m_SwapchainTexture;
        Device* m_Device;
    };
}
