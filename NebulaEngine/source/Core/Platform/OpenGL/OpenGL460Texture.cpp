#include "OpenGL460Texture.h"

#include <stb_image.h>

namespace Nebula
{
    OpenGL460Texture2D::OpenGL460Texture2D(const uint32_t width, const uint32_t height, InternalFormat internalFormat)
        : m_Width(width), m_Height(height)
    {
        NB_PROFILE_FUNCTION();

        m_InternalFormat = GetOpenGL460InternalFormat(internalFormat);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    OpenGL460Texture2D::OpenGL460Texture2D(const std::string& path)
        : m_Path(path)
    {
        NB_PROFILE_FUNCTION();

        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = nullptr;
        {
            NB_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        }
        NB_CORE_ASSERT(data, "Failed to load image!");
        m_Width = width;
        m_Height = height;

        GLenum dataFormat = 0;
        if (channels == 4) {
            m_InternalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        } else if (channels == 3) {
            m_InternalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }

        NB_CORE_ASSERT(m_InternalFormat & dataFormat, "Format not supported!");

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    OpenGL460Texture2D::~OpenGL460Texture2D()
    {
        NB_PROFILE_FUNCTION();

        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGL460Texture2D::SetData(void *data, uint32_t size, DataFormat format)
    {
        NB_PROFILE_FUNCTION();

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GetOpenGL460DataFormat(format), GL_UNSIGNED_BYTE, data);
    }

    std::vector<glm::vec4> OpenGL460Texture2D::GetDataRGBA()
    {
        int bufferSize = 4 * m_Width * m_Height;
        uint8_t* data = static_cast<uint8_t*>(malloc(bufferSize));

        glGetTextureImage(m_RendererID, 0, GL_RGBA, GL_UNSIGNED_BYTE, bufferSize, data);

        std::vector<glm::vec4> result(m_Width * m_Height);
        for (uint32_t y = 0; y < m_Height; y++)
        {
            for (uint32_t x = 0; x < m_Width; x++)
            {
                result[y*m_Width + x] = glm::vec4(data[((m_Height - y - 1)*m_Width + x)*4], data[((m_Height - y - 1)*m_Width + x)*4+1], data[((m_Height - y - 1)*m_Width + x)*4+2], data[((m_Height - y - 1)*m_Width + x)*4+3]);
            }
        }
        free(data);
        return result;
    }

    std::vector<uint8_t> OpenGL460Texture2D::GetDataR()
    {
        int bufferSize = m_Width * m_Height;
        uint8_t* data = static_cast<uint8_t*>(malloc(bufferSize));

        glGetTextureImage(m_RendererID, 0, GL_RED, GL_UNSIGNED_BYTE, bufferSize, data);

        std::vector<uint8_t> result(m_Width * m_Height);
        for (uint32_t y = 0; y < m_Height; y++)
        {
            for (uint32_t x = 0; x < m_Width; x++)
            {
                result[y*m_Width + x] = data[((m_Height - y - 1)*m_Width + x)];
            }
        }
        free(data);
        return result;
    }

    void OpenGL460Texture2D::Bind(uint32_t slot) const
    {
        NB_PROFILE_FUNCTION();

        glBindTextureUnit(slot, m_RendererID);
    }

    void OpenGL460Texture2D::BindToFrameBuffer(FramebufferSlotType slotType, uint32_t slot) const
    {
        GLenum attachment = 0;
        switch (slotType)
        {
        case FramebufferSlotType::Color: attachment = GL_COLOR_ATTACHMENT0 + slot; break;
        case FramebufferSlotType::Depth: attachment = GL_DEPTH_ATTACHMENT; break;
        case FramebufferSlotType::Stencil: attachment = GL_STENCIL_ATTACHMENT; break;
        case FramebufferSlotType::DepthStencil: attachment = GL_DEPTH_STENCIL_ATTACHMENT; break;
        }

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_RendererID, 0);
    }

    GLenum OpenGL460Texture2D::GetOpenGL460InternalFormat(InternalFormat format)
    {
        switch (format)
        {
        case InternalFormat::RGB8: return GL_RGB8;
        case InternalFormat::RGBA8: return GL_RGBA8;
        case InternalFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
        default:
            {
                NB_CORE_ASSERT(false, "Unknown InternalFormat!");
                return 0;
            }
        }
    }

    GLenum OpenGL460Texture2D::GetOpenGL460DataFormat(DataFormat format)
    {
        switch (format)
        {
        case DataFormat::R: return GL_RED;
        case DataFormat::RG: return GL_RG;
        case DataFormat::RGB: return GL_RGB;
        case DataFormat::BGR: return GL_BGR;
        case DataFormat::RGBA: return GL_RGBA;
        case DataFormat::BGRA: return GL_BGRA;
        case DataFormat::Depth: return GL_DEPTH_COMPONENT;
        case DataFormat::Stencil: return GL_STENCIL_INDEX;
        case DataFormat::DepthStencil: return GL_DEPTH_STENCIL;
        default:
            {
                NB_CORE_ASSERT(false, "Unknown DataFormat!");
                return 0;
            }
        }
    }
}
