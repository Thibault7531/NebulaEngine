#include "Metal3Framebuffer.h"

namespace Nebula::Graphics
{
    Metal3Framebuffer::Metal3Framebuffer(Device* device, const FramebufferSpecification& spec)
        : m_Spec(spec), m_Device{device}
    {
        for (Texture::TextureSpecification attachment : m_Spec.AttachmentSpec.Attachments)
        {
            if (IsDepthStencilFormat(attachment.InternalFormat)) m_DepthAttachmentSpec = attachment;
            else if (IsDepthFormat(attachment.InternalFormat)) m_DepthAttachmentSpec = attachment;
            else if (IsStencilFormat(attachment.InternalFormat)) {}
            else m_ColorAttachmentSpecs.push_back(attachment);
        }
        Invalidate();
    }

    Metal3Framebuffer::Metal3Framebuffer(Device* device, FramebufferSpecification framebufferSpec, uint32_t swapchainTextureIndex, Ref<Texture2D> swapchainTexture)
        : m_Spec(framebufferSpec), m_SwapchainTextureIndex(swapchainTextureIndex), m_SwapchainTexture(std::dynamic_pointer_cast<Metal3Texture2D>(swapchainTexture)), m_Device{device}
    {
        for (Texture::TextureSpecification attachment : m_Spec.AttachmentSpec.Attachments)
        {
            if (IsDepthStencilFormat(attachment.InternalFormat)) m_DepthAttachmentSpec = attachment;
            else if (IsDepthFormat(attachment.InternalFormat)) m_DepthAttachmentSpec = attachment;
            else if (IsStencilFormat(attachment.InternalFormat)) {}
            else m_ColorAttachmentSpecs.push_back(attachment);
        }
        Invalidate();
    }

    Metal3Framebuffer::~Metal3Framebuffer()
    {
        for (uint32_t attachment = 0; attachment < m_ColorAttachments.size(); attachment++) m_ColorAttachments[attachment].reset();
        m_DepthAttachment.reset();
    }

    void Metal3Framebuffer::Resize(uint32_t width, uint32_t height)
    {
        m_Spec.width = width;
        m_Spec.height = height;
        Invalidate();
    }

    void Metal3Framebuffer::Invalidate()
    {
        for (uint32_t attachment = 0; attachment < m_ColorAttachments.size(); attachment++) m_ColorAttachments[attachment].reset();
        m_DepthAttachment.reset();

        m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
        for (uint32_t attachment = 0; attachment < m_ColorAttachmentSpecs.size(); attachment++)
        {
            if (attachment == m_SwapchainTextureIndex) m_ColorAttachments[attachment] = m_SwapchainTexture;
            else m_ColorAttachments[attachment] = std::dynamic_pointer_cast<Metal3Texture2D>(m_Device->CreateTexture2D(m_Spec.width, m_Spec.height, m_ColorAttachmentSpecs[attachment]));
        }

        m_DepthAttachment = std::dynamic_pointer_cast<Metal3Texture2D>(m_Device->CreateTexture2D(m_Spec.width, m_Spec.height, m_DepthAttachmentSpec));
    }
}
