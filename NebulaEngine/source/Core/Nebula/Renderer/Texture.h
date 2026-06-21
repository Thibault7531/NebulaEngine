#pragma once

#include <glm.hpp>

#include "Nebula/CoreSystems/Core/Core.h"

namespace Nebula
{

    class Texture
    {
    public:
        enum class InternalFormat
        {
            None,
            RGB8,
            RGBA8,
            Depth24Stencil8
        };

        enum class DataFormat
        {
            R,
            RG,
            RGB,
            BGR,
            RGBA,
            BGRA,
            Depth,
            Stencil,
            DepthStencil
        };

        enum class DataType
        {
            UnsignedByte,
            UnsignedInt24_8
        };

        enum class FilterType
        {
            Nearest,
            Linear
        };

        enum class FramebufferSlotType
        {
            Color,
            Depth,
            Stencil,
            DepthStencil
        };

        struct TextureSpecification
        {
            InternalFormat InternalFormat = InternalFormat::RGBA8;
            FilterType MinFilter = FilterType::Linear;
            FilterType MagFilter = FilterType::Linear;

            TextureSpecification() = default;
            TextureSpecification(enum InternalFormat internalFormat)
                : InternalFormat(internalFormat) {}
        };

    public:
        virtual ~Texture() = default;

        [[nodiscard]] virtual uint32_t GetWidth() const = 0;
        [[nodiscard]] virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetRendererID() const = 0;

        virtual void SetData(void* data, uint32_t size, DataFormat format, DataType dataType = DataType::UnsignedByte) = 0;
        virtual std::vector<glm::vec4> GetDataRGBA() = 0;
        virtual std::vector<uint8_t> GetDataR() = 0;

        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual void BindToFrameBuffer(FramebufferSlotType slotType, uint32_t slot = 0) const = 0;
    };

    class Texture2D : public Texture
    {
    public:
        static Ref<Texture2D>Create(uint32_t width, uint32_t height, const TextureSpecification& spec);
        static Ref<Texture2D>Create(const std::string& path, const TextureSpecification& spec);


        virtual bool operator==(const Texture2D& other) const = 0;
    };

}
