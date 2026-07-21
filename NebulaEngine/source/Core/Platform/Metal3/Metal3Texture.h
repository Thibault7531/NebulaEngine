#pragma once

#include "Metal/Metal.hpp"
#include "Nebula/Graphics/Texture.h"

namespace Nebula::Graphics
{

    class Metal3Texture2D : public Texture2D
    {
    public:
        Metal3Texture2D(MTL::Device* device, uint32_t width, uint32_t height, const TextureSpecification& spec);
        Metal3Texture2D(MTL::Device* device, const std::filesystem::path& path, const TextureSpecification& spec);
        ~Metal3Texture2D();

        virtual TextureSpecification GetSpecification() override { return m_spec; }

        virtual uint32_t GetWidth() const override { return m_Texture->width(); }
        virtual uint32_t GetHeight() const override { return m_Texture->height(); }

        virtual void SetData(void* data, uint32_t size, DataFormat format, DataType dataType) override;
        virtual std::vector<uint8_t> GetDataR8() override;
        virtual std::vector<int32_t> GetDataR32I() override;
        virtual std::vector<uint32_t> GetDataR32UI() override;
        virtual std::vector<glm::vec4> GetDataRGBA8() override;

        virtual bool operator==(const Texture2D& other) const override;

    public:
        MTL::Texture* GetMetalTexture() const { return m_Texture; }
        MTL::SamplerState* GetMetalSampler() const { return m_Sampler; }

    public:
        static MTL::PixelFormat GetMetal3PixelFormat(InternalFormat format);
        static MTL::SamplerMinMagFilter GetMetal3MinMagFilter(FilterType filter);
        static uint32_t GetStrideFromDataFormat(DataFormat format);

    private:
        TextureSpecification m_spec;

        MTL::Texture* m_Texture;
        MTL::SamplerState* m_Sampler;
    };

}