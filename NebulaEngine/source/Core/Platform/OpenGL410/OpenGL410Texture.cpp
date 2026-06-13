#include "OpenGL410Texture.h"

#include <stb_image.h>

namespace Nebula
{
    OpenGL410Texture2D::OpenGL410Texture2D(const uint32_t width, const uint32_t height, InternalFormat internalFormat)
        : m_Width(width), m_Height(height)
    {
        NB_PROFILE_FUNCTION();

        m_InternalFormat = GetOpenGL410InternalFormat(internalFormat);
        
        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        //glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
            m_InternalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        } else if (channels == 3) {
            m_InternalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }

        NB_CORE_ASSERT(m_InternalFormat & dataFormat, "Format not supported!");

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    OpenGL410Texture2D::~OpenGL410Texture2D()
    {
        NB_PROFILE_FUNCTION();

        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGL410Texture2D::SetData(void *data, uint32_t size, DataFormat format)
    {
        NB_PROFILE_FUNCTION();

        glBindTexture(GL_TEXTURE_2D, m_RendererID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, GetOpenGL410DataFormat(format), GL_UNSIGNED_BYTE, data);
    }

    void OpenGL410Texture2D::Bind(uint32_t slot) const
    {
        NB_PROFILE_FUNCTION();

        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    GLenum OpenGL410Texture2D::GetOpenGL410InternalFormat(InternalFormat format)
    {
        switch (format)
        {
        case InternalFormat::RGB8: return GL_RGB8;
        case InternalFormat::RGBA8: return GL_RGBA8;
        default:
            {
                NB_CORE_ASSERT(false, "Unknown InternalFormat!");
                return 0;
            }
        }
    }

    GLenum OpenGL410Texture2D::GetOpenGL410DataFormat(DataFormat format)
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
        default:
            {
                NB_CORE_ASSERT(false, "Unknown DataFormat!");
                return 0;
            }
        }
    }
}
