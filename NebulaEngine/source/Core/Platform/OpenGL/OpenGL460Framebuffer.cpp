#include "OpenGL460Framebuffer.h"

#include <glad/gl.h>

namespace Nebula {

    OpenGL460Framebuffer::OpenGL460Framebuffer(const FramebufferSpecification &spec)
        : m_Specification(spec)
    {
        for (Texture::TextureSpecification format : m_Specification.AttachmentSpec.Attachments)
        {
            if (IsDepthFormat(format.InternalFormat)) m_DepthAttachmentSpec = format;
            else m_ColorAttachmentSpecs.push_back(format);
        }

        Invalidate();
    }

    OpenGL460Framebuffer::~OpenGL460Framebuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        for (uint32_t i = 0; i < m_ColorAttachments.size(); i++) m_ColorAttachments[i].reset();
    }

    void OpenGL460Framebuffer::Invalidate()
    {
        if (m_RendererID != 0) {
            glDeleteFramebuffers(1, &m_RendererID);
            for (uint32_t i = 0; i < m_ColorAttachments.size(); i++) m_ColorAttachments[i].reset();
        }

        if (m_Specification.width == 0 || m_Specification.height == 0)
        {
            m_Specification.width = 1;
            m_Specification.height = 1;
        }

        glCreateFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
        for (uint32_t i = 0; i < m_ColorAttachmentSpecs.size(); i++)
        {
            m_ColorAttachments[i] = Texture2D::Create(m_Specification.width, m_Specification.height, m_ColorAttachmentSpecs[i]);
            m_ColorAttachments[i]->BindToFrameBuffer(Texture::FramebufferSlotType::Color, i);
        }

        if (m_DepthAttachmentSpec.InternalFormat != Texture::InternalFormat::None)
        {
            m_DepthAttachment = Texture2D::Create(m_Specification.width, m_Specification.height, m_DepthAttachmentSpec);
            m_DepthAttachment->BindToFrameBuffer(Texture::FramebufferSlotType::DepthStencil);
        }

        if (m_ColorAttachments.size() > 0)
        {
            std::vector<GLenum> buffers{};
            for (uint32_t i = 0; i < m_ColorAttachments.size(); i++)
            {
                buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
            }
            glNamedFramebufferDrawBuffers(m_RendererID, buffers.size(), buffers.data());
        }
        else
        {
            glNamedFramebufferDrawBuffers(m_RendererID, 0, nullptr);
        }

        NB_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!")

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGL460Framebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Specification.width, m_Specification.height);
    }

    void OpenGL460Framebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGL460Framebuffer::Resize(uint32_t width, uint32_t height)
    {
        m_Specification.width = width;
        m_Specification.height = height;

        Invalidate();
    }

    bool OpenGL460Framebuffer::IsDepthFormat(Texture::InternalFormat format)
    {
        switch (format)
        {
        case Texture::InternalFormat::Depth24Stencil8: return true;
        default: return false;
        }
    }
}
