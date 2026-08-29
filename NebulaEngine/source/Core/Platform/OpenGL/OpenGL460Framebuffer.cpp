#include "OpenGL460Framebuffer.h"

#include <glad/gl.h>

#include "gtc/type_ptr.hpp"

namespace Nebula::Graphics {

    OpenGL460Framebuffer::OpenGL460Framebuffer(const FramebufferSpecification &spec)
        : m_Specification(spec)
    {
        for (Texture::TextureSpecification format : m_Specification.AttachmentSpec.Attachments)
        {
            if (IsDepthFormat(format.InternalFormat) || IsDepthStencilFormat(format.InternalFormat)) m_DepthAttachmentSpec = format;
            else m_ColorAttachmentSpecs.push_back(format);
        }

        m_ClearColors.resize(m_ColorAttachmentSpecs.size());

        Invalidate();
    }

    OpenGL460Framebuffer::~OpenGL460Framebuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        for (uint32_t i = 0; i < m_ColorAttachments.size(); i++) m_ColorAttachments[i].reset();
        m_DepthAttachment.reset();
    }

    void OpenGL460Framebuffer::Invalidate()
    {
        if (m_RendererID != 0) {
            glDeleteFramebuffers(1, &m_RendererID);
            for (uint32_t i = 0; i < m_ColorAttachments.size(); i++) m_ColorAttachments[i].reset();
            m_DepthAttachment.reset();
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
            m_ColorAttachments[i] = CreateRef<OpenGL460Texture2D>(m_Specification.width, m_Specification.height, m_ColorAttachmentSpecs[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorAttachments[i]->GetTextureID(), 0);
        }

        if (m_DepthAttachmentSpec.InternalFormat != Texture::InternalFormat::None)
        {
            m_DepthAttachment = CreateRef<OpenGL460Texture2D>(m_Specification.width, m_Specification.height, m_DepthAttachmentSpec);
            glFramebufferTexture2D(GL_FRAMEBUFFER, IsDepthStencilFormat(m_DepthAttachmentSpec.InternalFormat) ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment->GetTextureID(), 0);
        }

        if (m_ColorAttachments.size() > 0)
        {
            std::vector<GLenum> buffers(m_ColorAttachments.size());
            for (uint32_t i = 0; i < m_ColorAttachments.size(); i++)
            {
                buffers[i] = GL_COLOR_ATTACHMENT0 + i;
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

    void OpenGL460Framebuffer::Resize(uint32_t width, uint32_t height)
    {
        m_Specification.width = width;
        m_Specification.height = height;

        Invalidate();
    }

    void OpenGL460Framebuffer::Clear()
    {
        glDepthMask(GL_TRUE);

        for (uint32_t i = 0; i < m_ColorAttachments.size(); i++)
        {
            switch (GetColorAttachmentDataType(m_ColorAttachmentSpecs[i].InternalFormat))
            {
            case ColorAttachementDataType::None: break;
            case ColorAttachementDataType::Int:
                {
                    auto data = glm::ivec4(m_ClearColors[i]);
                    glClearNamedFramebufferiv(m_RendererID, GL_COLOR, i, glm::value_ptr(data));
                    break;
                }
            case ColorAttachementDataType::UnsignedInt:
                {
                    auto data = glm::uvec4(m_ClearColors[i]);
                    glClearNamedFramebufferuiv(m_RendererID, GL_COLOR, i, glm::value_ptr(data));
                    break;
                }
            case ColorAttachementDataType::Float: glClearNamedFramebufferfv(m_RendererID, GL_COLOR, i, glm::value_ptr(m_ClearColors[i])); break;
            }
        }
        if (m_DepthAttachment)
        {
            if (IsDepthStencilFormat(m_DepthAttachmentSpec.InternalFormat)) glClearNamedFramebufferfi(m_RendererID, GL_DEPTH_STENCIL, 0, m_ClearDepth, m_ClearStencil);
            else glClearNamedFramebufferfv(m_RendererID, GL_DEPTH, 0, &m_ClearDepth);
        }
    }

    ColorAttachementDataType OpenGL460Framebuffer::GetColorAttachmentDataType(Texture::InternalFormat format)
    {
        switch (format)
        {
        case Texture::InternalFormat::None: return ColorAttachementDataType::None;
        case Texture::InternalFormat::RGB8: return ColorAttachementDataType::Float;
        case Texture::InternalFormat::RGBA8: return ColorAttachementDataType::Float;
        case Texture::InternalFormat::R32I: return ColorAttachementDataType::Int;
        case Texture::InternalFormat::R32UI: return ColorAttachementDataType::UnsignedInt;
        default: return ColorAttachementDataType::None;
        }
    }
}
