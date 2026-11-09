#include "VulkanTexture.h"

#include "stb_image.h"
#include "Nebula/Debug/Instrumentor.h"

namespace Nebula::Graphics
{
    VulkanTexture2D::VulkanTexture2D(Device* device, uint32_t width, uint32_t height,
        const TextureSpecification& textureSpec)
            : m_Spec{textureSpec}, m_Width{width}, m_Height{height}, m_Device{dynamic_cast<VulkanDevice*>(device)}, m_OwnsImage{true}
    {
        vk::Extent3D imageExtent;
        imageExtent.width = width;
        imageExtent.height = height;
        imageExtent.depth = 1;

        vk::ImageCreateInfo imageCreateInfo;
        imageCreateInfo.sType = vk::StructureType::eImageCreateInfo;
        imageCreateInfo.imageType = vk::ImageType::e2D;
        imageCreateInfo.format = GetVulkanInternalFormat(m_Spec.InternalFormat);
        imageCreateInfo.extent = imageExtent;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
        imageCreateInfo.tiling = Framebuffer::IsDepthStencilFormat(textureSpec.InternalFormat) ? vk::ImageTiling::eOptimal : vk::ImageTiling::eLinear;
        imageCreateInfo.usage = vk::ImageUsageFlagBits::eSampled | (Framebuffer::IsDepthFormat(textureSpec.InternalFormat) ? vk::ImageUsageFlagBits::eDepthStencilAttachment : vk::ImageUsageFlagBits::eColorAttachment);
        imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        vma::Allocator allocator = m_Device->GetVMAAllocator();

        vma::AllocationCreateInfo allocInfo;
        allocInfo.usage = vma::MemoryUsage::eAuto;

        auto [allocation, image] = allocator.createImage(imageCreateInfo, allocInfo, &m_AllocationInfo);
        m_Allocation = allocation;
        m_Texture = image;
    }

    VulkanTexture2D::VulkanTexture2D(Device* device, const std::filesystem::path& path,
        const TextureSpecification& textureSpec)
            : m_Spec{textureSpec}, m_Device{dynamic_cast<VulkanDevice*>(device)}, m_OwnsImage{true}
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = nullptr;
        {
            NB_PROFILE_SCOPE("stbi_load - VulkanTexture2D::VulkanTexture2D(const std::string&)");
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        }
        NB_CORE_ASSERT(data, "Failed to load image!");

        m_Width = width;
        m_Height = height;
        vk::Format format = channels == 4 ? vk::Format::eR8G8B8A8Unorm : vk::Format::eR8G8B8Unorm;

        vk::Extent3D imageExtent;
        imageExtent.width = width;
        imageExtent.height = height;
        imageExtent.depth = 1;

        vk::ImageCreateInfo imageCreateInfo;
        imageCreateInfo.sType = vk::StructureType::eImageCreateInfo;
        imageCreateInfo.imageType = vk::ImageType::e2D;
        imageCreateInfo.format = format;
        imageCreateInfo.extent = imageExtent;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
        imageCreateInfo.tiling = vk::ImageTiling::eLinear;
        imageCreateInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eDepthStencilAttachment;
        imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        vma::Allocator allocator = m_Device->GetVMAAllocator();

        vma::AllocationCreateInfo allocInfo;
        allocInfo.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead | vma::AllocationCreateFlagBits::eMapped;
        allocInfo.usage = vma::MemoryUsage::eAuto;

        auto [allocation, image] = allocator.createImage(imageCreateInfo, allocInfo, &m_AllocationInfo);
        m_Allocation = allocation;
        m_Texture = image;

        memcpy(m_AllocationInfo.pMappedData, data, width * height * channels);

        stbi_image_free(data);
    }

    VulkanTexture2D::VulkanTexture2D(Device* device, uint32_t width, uint32_t height, vk::Image swapchainImage,
        const TextureSpecification& textureSpec)
            : m_Spec{textureSpec}, m_Width{width}, m_Height{height}, m_Device{dynamic_cast<VulkanDevice*>(device)}, m_OwnsImage{false}, m_Texture{swapchainImage}
    {}

    VulkanTexture2D::~VulkanTexture2D()
    {
        if (m_OwnsImage)
        {
            vma::Allocator allocator = m_Device->GetVMAAllocator();
            allocator.destroyImage(m_Texture, m_Allocation);
        }
    }

    void VulkanTexture2D::SetData(void* data, uint32_t size, DataFormat format, DataType dataType)
    {
        vk::DeviceSize imageSize = m_Width * m_Height * GetVulkanBytesPerPixel(m_Spec.InternalFormat);

        vk::BufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
        bufferCreateInfo.size = imageSize;
        bufferCreateInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
        bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        vma::AllocationCreateInfo allocationCreateInfo;
        allocationCreateInfo.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead | vma::AllocationCreateFlagBits::eMapped;
        allocationCreateInfo.usage = vma::MemoryUsage::eAuto;

        vk::Buffer stagingBuffer;
        vma::Allocation stagingBufferAllocation;
        vma::AllocationInfo stagingBufferAllocationInfo;
        auto [allocation, buffer] = m_Device->GetVMAAllocator().createBuffer(bufferCreateInfo, allocationCreateInfo, &stagingBufferAllocationInfo);
        stagingBuffer = buffer;
        stagingBufferAllocation = allocation;

        memcpy(stagingBufferAllocationInfo.pMappedData, data, size);

        NB_CORE_ASSERT(false, "Texture data upload not implemented yet! This will require command buffers and a transfer queue.");

        m_Device->GetVMAAllocator().destroyBuffer(stagingBuffer, stagingBufferAllocation);
    }

    std::vector<uint8_t> VulkanTexture2D::GetDataR8()
    {
        std::vector<uint8_t> data(m_Width * m_Height);
        memcpy(data.data(), m_AllocationInfo.pMappedData, m_Width * m_Height * sizeof(uint8_t));
        return data;
    }

    std::vector<int32_t> VulkanTexture2D::GetDataR32I()
    {
        std::vector<int32_t> data(m_Width * m_Height);
        memcpy(data.data(), m_AllocationInfo.pMappedData, m_Width * m_Height * sizeof(int32_t));
        return data;
    }

    std::vector<uint32_t> VulkanTexture2D::GetDataR32UI()
    {
        std::vector<uint32_t> data(m_Width * m_Height);
        memcpy(data.data(), m_AllocationInfo.pMappedData, m_Width * m_Height * sizeof(uint32_t));
        return data;
    }

    std::vector<glm::vec4> VulkanTexture2D::GetDataRGBA8()
    {
        std::vector<glm::vec4> data(m_Width * m_Height);
        memcpy(data.data(), m_AllocationInfo.pMappedData, m_Width * m_Height * sizeof(glm::vec4));
        return data;
    }

    bool VulkanTexture2D::operator==(const Texture2D& other) const
    {
        VulkanTexture2D& nativeOther = (VulkanTexture2D&)other;
        return m_Texture == nativeOther.m_Texture;
    }

    vk::Format VulkanTexture2D::GetVulkanInternalFormat(InternalFormat format)
    {
        switch (format)
        {
        case InternalFormat::None: return vk::Format::eUndefined;
        case InternalFormat::RGB8: return vk::Format::eR8G8B8Unorm;
        case InternalFormat::RGBA8: return vk::Format::eR8G8B8A8Unorm;
        case InternalFormat::R32I: return vk::Format::eR32Sint;
        case InternalFormat::R32UI: return vk::Format::eR32Uint;
        case InternalFormat::Depth24Stencil8: return vk::Format::eD24UnormS8Uint;
        }
        return vk::Format::eUndefined;
    }

    uint32_t VulkanTexture2D::GetVulkanBytesPerPixel(InternalFormat format)
    {
        switch (format)
        {
        case InternalFormat::None: return 0;
        case InternalFormat::RGB8: return 3;
        case InternalFormat::RGBA8: return 4;
        case InternalFormat::R32I: return 4;
        case InternalFormat::R32UI: return 4;
        case InternalFormat::Depth24Stencil8: return 4;
        }
        return 0;
    }
}
