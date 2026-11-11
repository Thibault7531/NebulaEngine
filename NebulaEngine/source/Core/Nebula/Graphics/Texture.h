#pragma once

#include <glm.hpp>
#include <vector>

#include "Core/Nebula/CoreSystems/Core/Core.h"

namespace Nebula::Graphics
{
    class Texture
    {
    public:
        enum class InternalFormat
        {
            None,
            RGB8,
            RGBA8,
            R32I,
            R32UI,
            Depth24Stencil8,
            Depth32Stencil8
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
            Int,
            UnsignedInt,
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
            TextureSpecification(enum InternalFormat internalFormat, FilterType filter)
                : InternalFormat(internalFormat), MinFilter(filter), MagFilter(filter) {}
            TextureSpecification(enum InternalFormat internalFormat, FilterType minFilter, FilterType magFilter)
                : InternalFormat(internalFormat), MinFilter(minFilter), MagFilter(magFilter) {}
        };

    public:
        virtual ~Texture() = default;
    };

    class Texture2D : public Texture
    {
    public:
        virtual TextureSpecification GetSpecification() = 0;

        [[nodiscard]] virtual uint32_t GetWidth() const = 0;
        [[nodiscard]] virtual uint32_t GetHeight() const = 0;

        // TODO: Consider using function overloading for GetData
        virtual void SetData(void* data, uint32_t size, DataFormat format, DataType dataType = DataType::UnsignedByte) = 0;
        virtual std::vector<uint8_t> GetDataR8() = 0;
        virtual std::vector<glm::vec4> GetDataRGBA8() = 0;
        virtual std::vector<int32_t> GetDataR32I() = 0;
        virtual std::vector<uint32_t> GetDataR32UI() = 0;

        virtual bool operator==(const Texture2D& other) const = 0;
    };
}
