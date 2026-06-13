#pragma once

#include <glad/gl.h>

#include "Nebula/Renderer/Texture.h"

namespace Nebula
{

    class OpenGL410Texture2D : public Texture2D
    {
    public:
        OpenGL410Texture2D(const uint32_t width, const uint32_t height, InternalFormat internalFormat);
        OpenGL410Texture2D(const std::string& path);
        virtual ~OpenGL410Texture2D();

        [[nodiscard]] virtual uint32_t GetWidth() const override { return m_Width; }
        [[nodiscard]] virtual uint32_t GetHeight() const override { return m_Height; }
        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual void SetData(void* data, uint32_t size, DataFormat format) override;

        virtual void Bind(uint32_t slot = 0) const override;

        virtual bool operator==(const Texture2D& other) const override
        {
            return m_RendererID == dynamic_cast<const OpenGL410Texture2D&>(other).m_RendererID;
        }

    private:
        GLenum GetOpenGL410InternalFormat(InternalFormat format);
        GLenum GetOpenGL410DataFormat(DataFormat format);

    private:
        std::string m_Path;
        uint32_t m_Width, m_Height;
        GLenum m_InternalFormat;
        uint32_t m_RendererID;
    };

}
