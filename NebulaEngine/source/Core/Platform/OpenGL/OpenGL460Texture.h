#pragma once

#include <glad/gl.h>

#include "Nebula/Renderer/Texture.h"

namespace Nebula
{

    class OpenGL460Texture2D : public Texture2D
    {
    public:
        OpenGL460Texture2D(uint32_t width, uint32_t height, const TextureSpecification& spec);
        OpenGL460Texture2D(const std::string& path, const TextureSpecification& spec);
        virtual ~OpenGL460Texture2D();

        [[nodiscard]] virtual uint32_t GetWidth() const override { return m_Width; }
        [[nodiscard]] virtual uint32_t GetHeight() const override { return m_Height; }
        virtual uint32_t GetRendererID() const override { return m_RendererID; }

        virtual void SetData(void* data, uint32_t size, DataFormat format, DataType dataType) override;
        virtual std::vector<uint8_t> GetDataR8() override;
        virtual std::vector<glm::vec4> GetDataRGBA8() override;
        virtual std::vector<int32_t> GetDataR32I() override;
        virtual std::vector<uint32_t> GetDataR32UI() override;

        virtual void Bind(uint32_t slot = 0) const override;
        virtual void BindToFrameBuffer(FramebufferSlotType slotType, uint32_t slot) const override;

        virtual bool operator==(const Texture2D& other) const override
        {
            return m_RendererID == dynamic_cast<const OpenGL460Texture2D&>(other).m_RendererID;
        }

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
