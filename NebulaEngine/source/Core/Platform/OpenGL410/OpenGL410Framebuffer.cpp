#include "OpenGL410Framebuffer.h"

#include <glad/gl.h>

#include "Core/Nebula/CoreSystems/Core/Core.h"
#include "Core/Nebula/CoreSystems/Core/Log.h"

namespace Nebula {

    OpenGL410Framebuffer::OpenGL410Framebuffer(const FramebufferSpecification &spec)
        : m_Specification(spec)
    {
        Invalidate();
    }

    OpenGL410Framebuffer::~OpenGL410Framebuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        m_ColorTexture.reset();
        m_DepthTexture.reset();
    }

    void OpenGL410Framebuffer::Invalidate()
    {
        if (m_RendererID != 0) {
            glDeleteFramebuffers(1, &m_RendererID);
            m_ColorTexture.reset();
            m_DepthTexture.reset();
        }

        if (m_Specification.width == 0 || m_Specification.height == 0)
        {
            m_Specification.width = 1;
            m_Specification.height = 1;
        }

        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        m_ColorTexture = Texture2D::Create(m_Specification.width, m_Specification.height, Texture::InternalFormat::RGBA8);
        m_ColorTexture->BindToFrameBuffer(Texture::FramebufferSlotType::Color);

        m_DepthTexture = Texture2D::Create(m_Specification.width, m_Specification.height, Texture::InternalFormat::Depth24Stencil8);
        m_DepthTexture->BindToFrameBuffer(Texture::FramebufferSlotType::DepthStencil);

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

    void OpenGL410Framebuffer::Resize(uint32_t width, uint32_t height)
    {
        m_Specification.width = width;
        m_Specification.height = height;

        Invalidate();
    }
}
