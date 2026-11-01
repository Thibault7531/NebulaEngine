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
        virtual std::vector<uint8_t> GetDataR8() override;
        virtual std::vector<int32_t> GetDataR32I() override;
        virtual std::vector<uint32_t> GetDataR32UI() override;
        virtual std::vector<glm::vec4> GetDataRGBA8() override;

        bool operator==(const Texture2D& other) const override;

    public:
        static vk::Format GetVulkanInternalFormat(InternalFormat format);

    private:
        TextureSpecification m_Spec;
        uint32_t m_Width, m_Height;

        VulkanDevice* m_Device;
        bool m_OwnsImage = false;
        vma::Allocation m_Allocation;
        vma::AllocationInfo m_AllocationInfo;
        vk::Image m_Texture;
    };
}
