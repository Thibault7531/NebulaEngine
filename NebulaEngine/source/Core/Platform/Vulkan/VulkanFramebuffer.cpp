#include "VulkanFramebuffer.h"

namespace Nebula::Graphics
{
    VulkanFramebuffer::VulkanFramebuffer(Device* device, FramebufferSpecification framebufferSpec)
        : m_Spec{framebufferSpec}, m_Device{device}
    {
        CheckFormatSupport();
        for (Texture::TextureSpecification attachment : m_Spec.AttachmentSpec.Attachments)
        {
            if (IsDepthStencilFormat(attachment.InternalFormat)) m_DepthAttachmentSpec = attachment;
            else if (IsDepthFormat(attachment.InternalFormat)) m_DepthAttachmentSpec = attachment;
            else if (IsStencilFormat(attachment.InternalFormat)) {}
            else m_ColorAttachmentSpecs.push_back(attachment);
        }
        Invalidate();
    }

    VulkanFramebuffer::VulkanFramebuffer(Device* device, FramebufferSpecification framebufferSpec, uint32_t swapchainTextureIndex,
        Ref<Texture2D> swapchainTexture)
            : m_Spec{framebufferSpec}, m_SwapchainTextureIndex{swapchainTextureIndex}, m_SwapchainTexture{std::dynamic_pointer_cast<VulkanTexture2D>(swapchainTexture)}, m_Device{device}
    {
        CheckFormatSupport();
        for (Texture::TextureSpecification attachment : m_Spec.AttachmentSpec.Attachments)
        {
            if (IsDepthStencilFormat(attachment.InternalFormat)) m_DepthAttachmentSpec = attachment;
            else if (IsDepthFormat(attachment.InternalFormat)) m_DepthAttachmentSpec = attachment;
            else if (IsStencilFormat(attachment.InternalFormat)) {}
            else m_ColorAttachmentSpecs.push_back(attachment);
        }
        Invalidate();
    }

    VulkanFramebuffer::~VulkanFramebuffer()
    {
        for (uint32_t attachment = 0; attachment < m_ColorAttachments.size(); attachment++) m_ColorAttachments[attachment].reset();
        m_DepthAttachment.reset();
    }

    void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
    {
        m_Spec.width = width;
        m_Spec.height = height;
        Invalidate();
    }

    void VulkanFramebuffer::CheckFormatSupport()
    {
        for (auto& attachmentSpec : m_Spec.AttachmentSpec.Attachments)
        {
            switch (attachmentSpec.InternalFormat)
            {
                case Texture::InternalFormat::Depth24Stencil8:
                    {
                        VulkanDevice* vulkanDevice = dynamic_cast<VulkanDevice*>(m_Device);
                        vk::FormatProperties formatProperties = vulkanDevice->GetVKPhysicalDevice().getFormatProperties(vk::Format::eD24UnormS8Uint);
                        if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment))
                        {
                            attachmentSpec.InternalFormat = Texture::InternalFormat::Depth32Stencil8;
                            CheckFormatSupport();
                        }
                    }
            }
        }
    }

    void VulkanFramebuffer::Invalidate()
    {
        for (uint32_t attachment = 0; attachment < m_ColorAttachments.size(); attachment++) m_ColorAttachments[attachment].reset();
        m_DepthAttachment.reset();

        m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
        for (uint32_t attachment = 0; attachment < m_ColorAttachmentSpecs.size(); attachment++)
        {
            if (attachment == m_SwapchainTextureIndex) m_ColorAttachments[attachment] = m_SwapchainTexture;
            else m_ColorAttachments[attachment] = std::dynamic_pointer_cast<VulkanTexture2D>(m_Device->CreateTexture2D(m_Spec.width, m_Spec.height, m_ColorAttachmentSpecs[attachment]));
        }

        m_DepthAttachment = std::dynamic_pointer_cast<VulkanTexture2D>(m_Device->CreateTexture2D(m_Spec.width, m_Spec.height, m_DepthAttachmentSpec));
    }
}
