#include "OpenGL410Framebuffer.h"

#include <glad/gl.h>

#include "Core/Nebula/CoreSystems/Core/Core.h"
#include "Core/Nebula/CoreSystems/Core/Log.h"
#include "gtc/type_ptr.hpp"

namespace Nebula {

    OpenGL410Framebuffer::OpenGL410Framebuffer(const FramebufferSpecification &spec)
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

    OpenGL410Framebuffer::~OpenGL410Framebuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        for (auto& m_ColorAttachment : m_ColorAttachments) m_ColorAttachment.reset();
    }

    void OpenGL410Framebuffer::Invalidate()
    {
        if (m_RendererID != 0) {
            glDeleteFramebuffers(1, &m_RendererID);
            for (auto& m_ColorAttachment : m_ColorAttachments) m_ColorAttachment.reset();
        }

        if (m_Specification.width == 0 || m_Specification.height == 0)
        {
            m_Specification.width = 1;
            m_Specification.height = 1;
        }

        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
        for (uint32_t i = 0; i < m_ColorAttachmentSpecs.size(); i++)
        {
            m_ColorAttachments[i] = Texture2D::Create(m_Specification.width, m_Specification.height, m_ColorAttachmentSpecs[i]);
            m_ColorAttachments[i]->BindToFrameBuffer(Texture::FramebufferSlotType::Color, i);
        }

        if (m_DepthAttachmentSpec.InternalFormat != Texture::InternalFormat::None)
        {
            m_DepthAttachment = Texture2D::Create(m_Specification.width, m_Specification.height, Texture::InternalFormat::Depth24Stencil8);
            m_DepthAttachment->BindToFrameBuffer(Texture::FramebufferSlotType::DepthStencil);
        }

        if (!m_ColorAttachments.empty())
        {
            std::vector<GLenum> buffers(m_ColorAttachments.size());
            for (uint32_t i = 0; i < m_ColorAttachments.size(); i++)
            {
                buffers[i] = GL_COLOR_ATTACHMENT0 + i;
            }
            glDrawBuffers(buffers.size(), buffers.data());
        }
        else
        {
            glDrawBuffers(0, nullptr);
        }

        NB_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!")

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGL410Framebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Specification.width, m_Specification.height);
    }

    void OpenGL410Framebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGL410Framebuffer::Clear()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        for (uint32_t i = 0; i < m_ColorAttachments.size(); i++)
        {
            switch (GetColorAttachmentDataType(m_ColorAttachmentSpecs[i].InternalFormat))
            {
            case ColorAttachementDataType::None: break;
            case ColorAttachementDataType::Int:
                {
                    auto data = glm::ivec4(m_ClearColors[i]);
                    glClearBufferiv(GL_COLOR, i, glm::value_ptr(data));
                    break;
                }
            case ColorAttachementDataType::UnsignedInt:
                {
                    auto data = glm::uvec4(m_ClearColors[i]);
                    glClearBufferuiv(GL_COLOR, i, glm::value_ptr(data));
                    break;
                }
            case ColorAttachementDataType::Float: glClearBufferfv(GL_COLOR, i, glm::value_ptr(m_ClearColors[i])); break;
            }
        }
        if (m_DepthAttachment) glClearBufferfv(GL_DEPTH, 0, &m_ClearDepth);
        if (m_DepthAttachment) glClearBufferiv(GL_STENCIL, 0, &m_ClearStencil);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGL410Framebuffer::Resize(uint32_t width, uint32_t height)
    {
        m_Specification.width = width;
        m_Specification.height = height;

        Invalidate();
    }

    ColorAttachementDataType OpenGL410Framebuffer::GetColorAttachmentDataType(Texture::InternalFormat format)
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
