#pragma once

#include <filesystem>
#include <glad/gl.h>

#include "Nebula/Graphics/Texture.h"

namespace Nebula::Graphics
{

    class OpenGL460Texture2D : public Texture2D
    {
    public:
        OpenGL460Texture2D(uint32_t width, uint32_t height, const TextureSpecification& spec);
        OpenGL460Texture2D(const std::filesystem::path& path, const TextureSpecification& spec);
        virtual ~OpenGL460Texture2D();

        virtual TextureSpecification GetSpecification() override { return m_Specification; }

        [[nodiscard]] virtual uint32_t GetWidth() const override { return m_Width; }
        [[nodiscard]] virtual uint32_t GetHeight() const override { return m_Height; }

        virtual void SetData(void* data, uint32_t size, DataFormat format, DataType dataType) override;
        virtual std::vector<uint8_t> GetRawData() override;

        virtual bool operator==(const Texture2D& other) const override
        {
            return m_RendererID == dynamic_cast<const OpenGL460Texture2D&>(other).m_RendererID;
        }

    public:
        GLuint GetTextureID() const { return m_RendererID; }

    private:
        GLenum GetOpenGL460InternalFormat(InternalFormat format);
        GLenum GetOpenGL460DataFormat(DataFormat format);
        GLenum GetOpenGL460DataType(DataType dataType);

        GLint GetOpenGL460FilterType(FilterType filterType);

    private:
        std::string m_Path;
        uint32_t m_Width, m_Height;
        TextureSpecification m_Specification;
        uint32_t m_RendererID;
    };

}
