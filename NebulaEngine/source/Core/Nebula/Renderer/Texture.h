#pragma once

#include "Nebula/CoreSystems/Core/Core.h"

namespace Nebula
{

    class Texture
    {
    public:
        enum class InternalFormat
        {
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

        enum class FramebufferSlotType
        {
            Color,
            Depth,
            Stencil,
            DepthStencil
        };

    public:
        virtual ~Texture() = default;

        [[nodiscard]] virtual uint32_t GetWidth() const = 0;
        [[nodiscard]] virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetRendererID() const = 0;

        virtual void SetData(void* data, uint32_t size, DataFormat format) = 0;

        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual void BindToFrameBuffer(FramebufferSlotType slotType, uint32_t slot = 0) const = 0;
    };

    class Texture2D : public Texture
    {
    public:
        static Ref<Texture2D>Create(uint32_t width, uint32_t height, InternalFormat internalFormat = InternalFormat::RGBA8);
        static Ref<Texture2D>Create(const std::string& path);

        virtual bool operator==(const Texture2D& other) const = 0;
    };

}
