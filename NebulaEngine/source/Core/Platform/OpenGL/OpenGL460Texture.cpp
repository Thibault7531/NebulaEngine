#include "OpenGL460Texture.h"

#include <stb_image.h>

namespace Nebula::Graphics
{

    OpenGL460Texture2D::OpenGL460Texture2D(uint32_t width, uint32_t height, const TextureSpecification& spec)
        : m_Width{width}, m_Height{height}, m_Specification{spec}
    {
        NB_PROFILE_FUNCTION();

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, GetOpenGL460InternalFormat(m_Specification.InternalFormat), m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GetOpenGL460FilterType(m_Specification.MinFilter));
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GetOpenGL460FilterType(m_Specification.MagFilter));
    }

    OpenGL460Texture2D::OpenGL460Texture2D(const std::filesystem::path& path, const TextureSpecification& spec)
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

        NB_CORE_ASSERT(GetOpenGL460InternalFormat(m_Specification.InternalFormat) & dataFormat, "Format not supported!");

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, GetOpenGL460InternalFormat(m_Specification.InternalFormat), m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GetOpenGL460FilterType(m_Specification.MinFilter));
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GetOpenGL460FilterType(m_Specification.MagFilter));

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    OpenGL460Texture2D::~OpenGL460Texture2D()
    {
        NB_PROFILE_FUNCTION();

        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGL460Texture2D::SetData(void *data, uint32_t size, DataFormat format, DataType dataType)
    {
        NB_PROFILE_FUNCTION();

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, GetOpenGL460DataFormat(format), GetOpenGL460DataType(dataType), data);
    }

    std::vector<uint8_t> OpenGL460Texture2D::GetDataR8()
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

    std::vector<glm::vec4> OpenGL460Texture2D::GetDataRGBA8()
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

    std::vector<int32_t> OpenGL460Texture2D::GetDataR32I()
    {
        int bufferSize = 4 * m_Width * m_Height;
        int32_t* data = static_cast<int32_t*>(malloc(bufferSize));

        glGetTextureImage(m_RendererID, 0, GL_RED_INTEGER, GL_INT, bufferSize, data);

        std::vector<int32_t> result(m_Width * m_Height);
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

    std::vector<uint32_t> OpenGL460Texture2D::GetDataR32UI()
    {
        int bufferSize = 4 * m_Width * m_Height;
        uint32_t* data = static_cast<uint32_t*>(malloc(bufferSize));

        glGetTextureImage(m_RendererID, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, bufferSize, data);

        std::vector<uint32_t> result(m_Width * m_Height);
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

    GLenum OpenGL460Texture2D::GetOpenGL460InternalFormat(InternalFormat format)
    {
        switch (format)
        {
        case InternalFormat::RGB8: return GL_RGB8;
        case InternalFormat::RGBA8: return GL_RGBA8;
        case InternalFormat::R32I: return GL_R32I;
        case InternalFormat::R32UI: return GL_R32UI;
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

    GLenum OpenGL460Texture2D::GetOpenGL460DataType(DataType dataType)
    {
        switch (dataType)
        {
        case DataType::UnsignedByte: return GL_UNSIGNED_BYTE;
        case DataType::Int: return GL_INT;
        case DataType::UnsignedInt: return GL_UNSIGNED_INT;
        case DataType::UnsignedInt24_8: return GL_UNSIGNED_INT_24_8;
        default:
            {
                NB_CORE_ASSERT(false, "Unknown DataType!");
                return 0;
            }
        }
    }

    GLint OpenGL460Texture2D::GetOpenGL460FilterType(FilterType filterType)
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
