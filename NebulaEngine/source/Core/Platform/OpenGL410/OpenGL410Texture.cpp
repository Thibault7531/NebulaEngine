#include "OpenGL410Texture.h"

#include <stb_image.h>

namespace Nebula
{
    OpenGL410Texture2D::OpenGL410Texture2D(const uint32_t width, const uint32_t height, InternalFormat internalFormat)
        : m_Width(width), m_Height(height)
    {
        NB_PROFILE_FUNCTION();

        m_Specification.InternalFormat = internalFormat;
        
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexImage2D(GL_TEXTURE_2D, 0, GetOpenGL410InternalFormat(m_Specification.InternalFormat), m_Width, m_Height, 0, GetDataFormatMatchingInternalFormat(), GetDataTypeMatchingInternalFormat(), nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    OpenGL410Texture2D::OpenGL410Texture2D(uint32_t width, uint32_t height, const TextureSpecification& spec)
        : m_Width{width}, m_Height{height}, m_Specification{spec}
    {
        NB_PROFILE_FUNCTION();

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexImage2D(GL_TEXTURE_2D, 0, GetOpenGL410InternalFormat(m_Specification.InternalFormat), m_Width, m_Height, 0, GetDataFormatMatchingInternalFormat(), GetDataTypeMatchingInternalFormat(), nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetOpenGL410FilterType(m_Specification.MinFilter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetOpenGL410FilterType(m_Specification.MagFilter));
    }

    OpenGL410Texture2D::OpenGL410Texture2D(const std::string& path)
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
            m_Specification.InternalFormat = InternalFormat::RGBA8;
            dataFormat = GL_RGBA;
        } else if (channels == 3) {
            m_Specification.InternalFormat = InternalFormat::RGB8;
            dataFormat = GL_RGB;
        }

        NB_CORE_ASSERT(GetOpenGL410InternalFormat(m_Specification.InternalFormat) & dataFormat, "Format not supported!");

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexImage2D(GL_TEXTURE_2D, 0, GetOpenGL410InternalFormat(m_Specification.InternalFormat), m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    OpenGL410Texture2D::OpenGL410Texture2D(const std::string& path, const TextureSpecification& spec)
        : m_Path{path}, m_Specification{spec}
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
            m_Specification.InternalFormat = InternalFormat::RGBA8;
            dataFormat = GL_RGBA;
        } else if (channels == 3) {
            m_Specification.InternalFormat = InternalFormat::RGB8;
            dataFormat = GL_RGB;
        }

        NB_CORE_ASSERT(GetOpenGL410InternalFormat(m_Specification.InternalFormat) & dataFormat, "Format not supported!");

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexImage2D(GL_TEXTURE_2D, 0, GetOpenGL410InternalFormat(m_Specification.InternalFormat), m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, nullptr);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GetOpenGL410FilterType(m_Specification.MinFilter));
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GetOpenGL410FilterType(m_Specification.MagFilter));

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    OpenGL410Texture2D::~OpenGL410Texture2D()
    {
        NB_PROFILE_FUNCTION();

        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGL410Texture2D::SetData(void *data, uint32_t size, DataFormat format, DataType dataType)
    {
        NB_PROFILE_FUNCTION();

        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, GetOpenGL410DataFormat(format), GetOpenGL410DataType(dataType), data);
    }

    std::vector<glm::vec4> OpenGL410Texture2D::GetDataRGBA()
    {
        int bufferSize = 4 * m_Width * m_Height;
        uint8_t* data = static_cast<uint8_t*>(malloc(bufferSize));

        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);

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

    std::vector<uint8_t> OpenGL410Texture2D::GetDataR()
    {
        int bufferSize = m_Width * m_Height;
        uint8_t* data = static_cast<uint8_t*>(malloc(bufferSize));

        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);

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

    void OpenGL410Texture2D::Bind(uint32_t slot) const
    {
        NB_PROFILE_FUNCTION();

        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void OpenGL410Texture2D::BindToFrameBuffer(FramebufferSlotType slotType, uint32_t slot) const
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

    GLint OpenGL410Texture2D::GetOpenGL410InternalFormat(InternalFormat format)
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

    GLint OpenGL410Texture2D::GetOpenGL410DataFormat(DataFormat format)
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

    GLint OpenGL410Texture2D::GetOpenGL410DataType(DataType dataType)
    {
        switch (dataType)
        {
        case DataType::UnsignedByte: return GL_UNSIGNED_BYTE;
        case DataType::UnsignedInt24_8: return GL_UNSIGNED_INT_24_8;
        default:
            {
                NB_CORE_ASSERT(false, "Unknown DataType!");
                return 0;
            }
        }
    }

    GLint OpenGL410Texture2D::GetDataFormatMatchingInternalFormat()
    {
        switch (m_Specification.InternalFormat)
        {
        case InternalFormat::RGB8: return GL_RGB;
        case InternalFormat::RGBA8: return GL_RGBA;
        case InternalFormat::Depth24Stencil8: return GL_DEPTH_STENCIL;
        }
    }

    GLint OpenGL410Texture2D::GetDataTypeMatchingInternalFormat()
    {
        switch (m_Specification.InternalFormat)
        {
        case InternalFormat::Depth24Stencil8: return GL_UNSIGNED_INT_24_8;
        default: return GL_UNSIGNED_BYTE;
        }
    }

    GLint OpenGL410Texture2D::GetOpenGL410FilterType(FilterType filterType)
    {
        switch (filterType)
        {
        case FilterType::Nearest: return GL_NEAREST;
        case FilterType::Linear: return GL_LINEAR;
        default:
            {
                NB_CORE_ASSERT(false, "Unknown FilterType!");
                return 0;
            }
        }
    }
}
