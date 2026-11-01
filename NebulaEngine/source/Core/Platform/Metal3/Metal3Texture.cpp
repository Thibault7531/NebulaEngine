#include "Metal3Texture.h"

#include <stb_image.h>

#include "Nebula/Graphics/Buffers/Framebuffer.h"

namespace Nebula::Graphics
{
    Metal3Texture2D::Metal3Texture2D(MTL::Device* device, uint32_t width, uint32_t height,
        const TextureSpecification& spec)
            : m_spec{spec}, m_Device{device}
    {
        MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
        textureDescriptor->setTextureType(MTL::TextureType2D);
        textureDescriptor->setPixelFormat(GetMetal3PixelFormat(m_spec.InternalFormat));
        textureDescriptor->setWidth(width);
        textureDescriptor->setHeight(height);
        textureDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        if (Framebuffer::IsDepthStencilFormat(spec.InternalFormat) || Framebuffer::IsDepthFormat(spec.InternalFormat) || Framebuffer::IsStencilFormat(spec.InternalFormat))
        {
            textureDescriptor->setStorageMode(MTL::StorageModePrivate);
        }

        MTL::SamplerDescriptor* samplerDescriptor = MTL::SamplerDescriptor::alloc()->init();
        samplerDescriptor->setNormalizedCoordinates(true);
        samplerDescriptor->setRAddressMode(MTL::SamplerAddressModeRepeat);
        samplerDescriptor->setSAddressMode(MTL::SamplerAddressModeRepeat);
        samplerDescriptor->setTAddressMode(MTL::SamplerAddressModeRepeat);
        samplerDescriptor->setMinFilter(GetMetal3MinMagFilter(m_spec.MinFilter));
        samplerDescriptor->setMagFilter(GetMetal3MinMagFilter(m_spec.MagFilter));

        m_Texture = device->newTexture(textureDescriptor);
        m_Sampler = device->newSamplerState(samplerDescriptor);

        textureDescriptor->release();
        samplerDescriptor->release();
    }

    Metal3Texture2D::Metal3Texture2D(MTL::Device* device, const std::filesystem::path& path,
        const TextureSpecification& spec)
            : m_spec{spec}, m_Device{device}
    {
        int width, height, channels, components;
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = nullptr;
        {
            NB_PROFILE_SCOPE("stbi_load - Metal3Texture2D::Metal3Texture2D(const std::string&)");
            stbi_info(path.c_str(), &width, &height, &components);
            data = stbi_load(path.c_str(), &width, &height, &channels, components == 3 ? 4 : 0);
            if (components == 3) channels = 4;
        }
        NB_CORE_ASSERT(data, "Failed to load image!");

        MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
        textureDescriptor->setTextureType(MTL::TextureType2D);
        textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
        textureDescriptor->setWidth(width);
        textureDescriptor->setHeight(height);
        textureDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);

        MTL::SamplerDescriptor* samplerDescriptor = MTL::SamplerDescriptor::alloc()->init();
        samplerDescriptor->setNormalizedCoordinates(true);
        samplerDescriptor->setRAddressMode(MTL::SamplerAddressModeRepeat);
        samplerDescriptor->setSAddressMode(MTL::SamplerAddressModeRepeat);
        samplerDescriptor->setTAddressMode(MTL::SamplerAddressModeRepeat);
        samplerDescriptor->setMinFilter(GetMetal3MinMagFilter(m_spec.MinFilter));
        samplerDescriptor->setMagFilter(GetMetal3MinMagFilter(m_spec.MagFilter));

        m_Texture = device->newTexture(textureDescriptor);
        m_Texture->replaceRegion(MTL::Region(0, 0, width, height), 0, data, channels * width);
        m_Sampler = device->newSamplerState(samplerDescriptor);

        textureDescriptor->release();
        samplerDescriptor->release();
        stbi_image_free(data);
    }

    Metal3Texture2D::~Metal3Texture2D()
    {
        m_Sampler->release();
        m_Texture->release();
    }

    void Metal3Texture2D::SetData(void* data, uint32_t size, DataFormat format, DataType dataType)
    {
        m_Texture->replaceRegion(MTL::Region(0, 0, m_Texture->width(), m_Texture->height()), 0, data, size/m_Texture->height());
    }

    std::vector<uint8_t> Metal3Texture2D::GetDataR8()
    {
        MTL::CommandQueue* commandQueue = m_Device->newCommandQueue();
        MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
        MTL::BlitCommandEncoder* blitEncoder = commandBuffer->blitCommandEncoder();
        blitEncoder->synchronizeTexture(m_Texture, 0, 0);
        blitEncoder->endEncoding();
        commandBuffer->commit();
        commandBuffer->waitUntilCompleted();
        commandQueue->release();

        std::vector<uint8_t> result(m_Texture->width() * m_Texture->height());
        m_Texture->getBytes(result.data(), m_Texture->width() * sizeof(uint8_t), MTL::Region(0, 0, m_Texture->width(), m_Texture->height()), 0);
        return result;
    }

    std::vector<int32_t> Metal3Texture2D::GetDataR32I()
    {
        MTL::CommandQueue* commandQueue = m_Device->newCommandQueue();
        MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
        MTL::BlitCommandEncoder* blitEncoder = commandBuffer->blitCommandEncoder();
        blitEncoder->synchronizeTexture(m_Texture, 0, 0);
        blitEncoder->endEncoding();
        commandBuffer->commit();
        commandBuffer->waitUntilCompleted();
        commandQueue->release();

        std::vector<int32_t> result(m_Texture->width() * m_Texture->height());
        m_Texture->getBytes(result.data(), m_Texture->width() * sizeof(int32_t), MTL::Region(0, 0, m_Texture->width(), m_Texture->height()), 0);
        return result;
    }

    std::vector<uint32_t> Metal3Texture2D::GetDataR32UI()
    {
        MTL::CommandQueue* commandQueue = m_Device->newCommandQueue();
        MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
        MTL::BlitCommandEncoder* blitEncoder = commandBuffer->blitCommandEncoder();
        blitEncoder->synchronizeTexture(m_Texture, 0, 0);
        blitEncoder->endEncoding();
        commandBuffer->commit();
        commandBuffer->waitUntilCompleted();
        commandQueue->release();

        std::vector<uint32_t> result(m_Texture->width() * m_Texture->height());
        m_Texture->getBytes(result.data(), m_Texture->width() * sizeof(uint32_t), MTL::Region(0, 0, m_Texture->width(), m_Texture->height()), 0);
        return result;
    }

    std::vector<glm::vec4> Metal3Texture2D::GetDataRGBA8()
    {
        MTL::CommandQueue* commandQueue = m_Device->newCommandQueue();
        MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
        MTL::BlitCommandEncoder* blitEncoder = commandBuffer->blitCommandEncoder();
        blitEncoder->synchronizeTexture(m_Texture, 0, 0);
        blitEncoder->endEncoding();
        commandBuffer->commit();
        commandBuffer->waitUntilCompleted();
        commandQueue->release();

        std::vector<glm::vec4> result(m_Texture->width() * m_Texture->height());
        m_Texture->getBytes(result.data(), m_Texture->width() * sizeof(glm::vec4), MTL::Region(0, 0, m_Texture->width(), m_Texture->height()), 0);
        return result;
    }

    bool Metal3Texture2D::operator==(const Texture2D& other) const
    {
        Metal3Texture2D& nativeOther = (Metal3Texture2D&)other;
        return m_Texture == nativeOther.m_Texture;
    }

    MTL::PixelFormat Metal3Texture2D::GetMetal3PixelFormat(InternalFormat format)
    {
        switch (format)
        {
        case InternalFormat::None: break;
        case InternalFormat::RGB8: return MTL::PixelFormatRGBA8Unorm;
        case InternalFormat::RGBA8: return MTL::PixelFormatRGBA8Unorm;
        case InternalFormat::R32I: return MTL::PixelFormatR32Sint;
        case InternalFormat::R32UI: return MTL::PixelFormatR32Uint;
        case InternalFormat::Depth24Stencil8: return MTL::PixelFormatDepth24Unorm_Stencil8;
        }
    }

    MTL::SamplerMinMagFilter Metal3Texture2D::GetMetal3MinMagFilter(FilterType filter)
    {
        switch (filter)
        {
        case FilterType::Nearest: return MTL::SamplerMinMagFilterNearest;
        case FilterType::Linear: return MTL::SamplerMinMagFilterLinear;
        }
    }

    uint32_t Metal3Texture2D::GetStrideFromDataFormat(DataFormat format)
    {
        switch (format)
        {
        case DataFormat::R: return 1;
        case DataFormat::RG: return 2;
        case DataFormat::RGB: return 3;
        case DataFormat::BGR: return 3;
        case DataFormat::RGBA: return 4;
        case DataFormat::BGRA: return 4;
        case DataFormat::Depth: return 4;
        case DataFormat::Stencil: return 1;
        case DataFormat::DepthStencil: return 4;
        }
    }
}
