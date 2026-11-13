#pragma once

#include <filesystem>

#include "vk_mem_alloc.hpp"
#include "VulkanDevice.h"
#include "Nebula/Graphics/Device.h"
#include "Nebula/Graphics/Texture.h"

namespace Nebula::Graphics
{
    class VulkanTexture2D : public Texture2D
    {
    public:
        VulkanTexture2D(Device* device, uint32_t width, uint32_t height, const TextureSpecification& textureSpec);
        VulkanTexture2D(Device* device, const std::filesystem::path& path, const TextureSpecification& textureSpec);
        VulkanTexture2D(Device* device, uint32_t width, uint32_t height, vk::Image swapchainImage, const TextureSpecification& textureSpec);
        ~VulkanTexture2D() override;

        virtual TextureSpecification GetSpecification() override { return m_Spec; }

        virtual uint32_t GetWidth() const override { return m_Width; }
        virtual uint32_t GetHeight() const override { return m_Height; }

        virtual void SetData(void* data, uint32_t size, DataFormat format, DataType dataType) override;
        virtual std::vector<uint8_t> GetRawData() override;

        bool operator==(const Texture2D& other) const override;

    public:
        vk::ImageLayout GetVKImageLayout() const { return m_TextureLayout; }
        void SetVKImageLayout(vk::ImageLayout layout) { m_TextureLayout = layout; }
        vk::Image GetVKImage() const { return m_Texture; }
        vk::ImageView GetVKImageView() const { return m_TextureView; }
        vk::Sampler GetVKSampler() const { return m_Sampler; }

        void CheckFormatSupport();

    public:
        static vk::Format GetVulkanInternalFormat(InternalFormat format);
        static uint32_t GetVulkanBytesPerPixel(InternalFormat format);
        static vk::Filter GetVulkanFilterType(FilterType filter);

    private:
        TextureSpecification m_Spec;
        uint32_t m_Width, m_Height;

        VulkanDevice* m_Device;
        bool m_OwnsImage = false;
        vma::Allocation m_Allocation;
        vma::AllocationInfo m_AllocationInfo;
        vk::Image m_Texture;
        vk::ImageView m_TextureView;
        vk::ImageLayout m_TextureLayout = vk::ImageLayout::eUndefined;
        vk::Sampler m_Sampler;
    };
}
