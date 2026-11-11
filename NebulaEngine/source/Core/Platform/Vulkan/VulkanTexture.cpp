#include "VulkanTexture.h"

#include "stb_image.h"
#include "VulkanQueue.h"
#include "Nebula/Debug/Instrumentor.h"

namespace Nebula::Graphics
{
    VulkanTexture2D::VulkanTexture2D(Device* device, uint32_t width, uint32_t height,
        const TextureSpecification& textureSpec)
            : m_Spec{textureSpec}, m_Width{width}, m_Height{height}, m_Device{dynamic_cast<VulkanDevice*>(device)}, m_OwnsImage{true}
    {
        CheckFormatSupport();

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
        imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        if (Framebuffer::IsDepthStencilFormat(textureSpec.InternalFormat))
        {
            imageCreateInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
        }
        else
        {
            imageCreateInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
            imageCreateInfo.tiling = vk::ImageTiling::eLinear;
        }

        vma::Allocator allocator = m_Device->GetVMAAllocator();

        vma::AllocationCreateInfo allocInfo;
        allocInfo.usage = vma::MemoryUsage::eAuto;

        auto [allocation, image] = allocator.createImage(imageCreateInfo, allocInfo, &m_AllocationInfo);
        m_Allocation = allocation;
        m_Texture = image;

        vk::ImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.sType = vk::StructureType::eImageViewCreateInfo;
        imageViewCreateInfo.image = m_Texture;
        imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
        imageViewCreateInfo.format = GetVulkanInternalFormat(m_Spec.InternalFormat);
        imageViewCreateInfo.subresourceRange = vk::ImageSubresourceRange(Framebuffer::IsDepthStencilFormat(textureSpec.InternalFormat) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        m_TextureView = m_Device->GetVKLogicalDevice().createImageView(imageViewCreateInfo);

        vk::SamplerCreateInfo samplerCreateInfo;
        samplerCreateInfo.sType = vk::StructureType::eSamplerCreateInfo;
        samplerCreateInfo.magFilter = GetVulkanFilterType(m_Spec.MagFilter);
        samplerCreateInfo.minFilter = GetVulkanFilterType(m_Spec.MinFilter);
        samplerCreateInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerCreateInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerCreateInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        m_Sampler = m_Device->GetVKLogicalDevice().createSampler(samplerCreateInfo);
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
            data = stbi_load(path.c_str(), &width, &height, &channels, 4);
        }
        NB_CORE_ASSERT(data, "Failed to load image!");

        m_Width = width;
        m_Height = height;
        m_Spec.InternalFormat = InternalFormat::RGBA8;
        CheckFormatSupport();
        vk::Format format = GetVulkanInternalFormat(m_Spec.InternalFormat);

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
        imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;

        if (Framebuffer::IsDepthStencilFormat(textureSpec.InternalFormat))
        {
            imageCreateInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
        }
        else
        {
            imageCreateInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
            imageCreateInfo.tiling = vk::ImageTiling::eLinear;
        }

        vma::Allocator allocator = m_Device->GetVMAAllocator();

        vma::AllocationCreateInfo allocInfo;
        allocInfo.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead | vma::AllocationCreateFlagBits::eMapped;
        allocInfo.usage = vma::MemoryUsage::eAuto;

        auto [allocation, image] = allocator.createImage(imageCreateInfo, allocInfo, &m_AllocationInfo);
        m_Allocation = allocation;
        m_Texture = image;

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
        auto [stagingAllocation, buffer] = m_Device->GetVMAAllocator().createBuffer(bufferCreateInfo, allocationCreateInfo, &stagingBufferAllocationInfo);
        stagingBuffer = buffer;
        stagingBufferAllocation = stagingAllocation;

        memcpy(stagingBufferAllocationInfo.pMappedData, data, imageSize);

        vk::ImageMemoryBarrier2 imageBarrier;
        imageBarrier.sType = vk::StructureType::eImageMemoryBarrier2;
        imageBarrier.oldLayout = m_TextureLayout;
        imageBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
        m_TextureLayout = vk::ImageLayout::eTransferDstOptimal;
        imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.image = m_Texture;
        imageBarrier.subresourceRange = vk::ImageSubresourceRange(Framebuffer::IsDepthStencilFormat(m_Spec.InternalFormat) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        imageBarrier.srcAccessMask = vk::AccessFlagBits2::eNone;
        imageBarrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;
        imageBarrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;

        Ref<VulkanQueue> graphicsQueue = std::dynamic_pointer_cast<VulkanQueue>(m_Device->GetGraphicsQueue());

        vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
        commandBufferAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
        commandBufferAllocateInfo.commandPool = graphicsQueue->GetVKCommandPool();
        commandBufferAllocateInfo.commandBufferCount = 1;

        vk::CommandBuffer commandBuffer = m_Device->GetVKLogicalDevice().allocateCommandBuffers(commandBufferAllocateInfo)[0];
        commandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

        vk::DependencyInfo dependencyInfo;
        dependencyInfo.sType = vk::StructureType::eDependencyInfo;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &imageBarrier;
        commandBuffer.pipelineBarrier2(dependencyInfo);

        vk::BufferImageCopy region;
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = Framebuffer::IsDepthStencilFormat(m_Spec.InternalFormat) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = vk::Offset3D(0, 0, 0);
        region.imageExtent = vk::Extent3D(m_Width, m_Height, 1);
        commandBuffer.copyBufferToImage(stagingBuffer, m_Texture, vk::ImageLayout::eTransferDstOptimal, 1, &region);

        imageBarrier.sType = vk::StructureType::eImageMemoryBarrier2;
        imageBarrier.oldLayout = m_TextureLayout;
        imageBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        m_TextureLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.image = m_Texture;
        imageBarrier.subresourceRange = vk::ImageSubresourceRange(Framebuffer::IsDepthStencilFormat(m_Spec.InternalFormat) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        imageBarrier.srcAccessMask = vk::AccessFlagBits2::eNone;
        imageBarrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;
        imageBarrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader;

        dependencyInfo.sType = vk::StructureType::eDependencyInfo;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &imageBarrier;
        commandBuffer.pipelineBarrier2(dependencyInfo);

        commandBuffer.end();

        vk::SubmitInfo submitInfo;
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        graphicsQueue->GetVKQueue().submit(submitInfo, nullptr);
        graphicsQueue->GetVKQueue().waitIdle();

        vk::ImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.sType = vk::StructureType::eImageViewCreateInfo;
        imageViewCreateInfo.image = m_Texture;
        imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
        imageViewCreateInfo.format = GetVulkanInternalFormat(m_Spec.InternalFormat);
        imageViewCreateInfo.subresourceRange = vk::ImageSubresourceRange(Framebuffer::IsDepthStencilFormat(textureSpec.InternalFormat) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        m_TextureView = m_Device->GetVKLogicalDevice().createImageView(imageViewCreateInfo);

        vk::SamplerCreateInfo samplerCreateInfo;
        samplerCreateInfo.sType = vk::StructureType::eSamplerCreateInfo;
        samplerCreateInfo.magFilter = GetVulkanFilterType(m_Spec.MagFilter);
        samplerCreateInfo.minFilter = GetVulkanFilterType(m_Spec.MinFilter);
        samplerCreateInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerCreateInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerCreateInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        m_Sampler = m_Device->GetVKLogicalDevice().createSampler(samplerCreateInfo);

        m_Device->GetVMAAllocator().destroyBuffer(stagingBuffer, stagingBufferAllocation);

        stbi_image_free(data);
    }

    VulkanTexture2D::VulkanTexture2D(Device* device, uint32_t width, uint32_t height, vk::Image swapchainImage,
        const TextureSpecification& textureSpec)
            : m_Spec{textureSpec}, m_Width{width}, m_Height{height}, m_Device{dynamic_cast<VulkanDevice*>(device)}, m_OwnsImage{false}, m_Texture{swapchainImage}
    {
        vk::ImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.sType = vk::StructureType::eImageViewCreateInfo;
        imageViewCreateInfo.image = m_Texture;
        imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
        imageViewCreateInfo.format = GetVulkanInternalFormat(m_Spec.InternalFormat);
        imageViewCreateInfo.subresourceRange = vk::ImageSubresourceRange(Framebuffer::IsDepthStencilFormat(textureSpec.InternalFormat) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        m_TextureView = m_Device->GetVKLogicalDevice().createImageView(imageViewCreateInfo);

        vk::SamplerCreateInfo samplerCreateInfo;
        samplerCreateInfo.sType = vk::StructureType::eSamplerCreateInfo;
        samplerCreateInfo.magFilter = GetVulkanFilterType(m_Spec.MagFilter);
        samplerCreateInfo.minFilter = GetVulkanFilterType(m_Spec.MinFilter);
        samplerCreateInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
        samplerCreateInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
        samplerCreateInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
        m_Sampler = m_Device->GetVKLogicalDevice().createSampler(samplerCreateInfo);
    }

    VulkanTexture2D::~VulkanTexture2D()
    {
        m_Device->GetVKLogicalDevice().destroySampler(m_Sampler);
        m_Device->GetVKLogicalDevice().destroyImageView(m_TextureView);
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

        vk::ImageMemoryBarrier2 imageBarrier;
        imageBarrier.sType = vk::StructureType::eImageMemoryBarrier2;
        imageBarrier.oldLayout = m_TextureLayout;
        imageBarrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
        m_TextureLayout = vk::ImageLayout::eTransferDstOptimal;
        imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageBarrier.image = m_Texture;
        imageBarrier.subresourceRange = vk::ImageSubresourceRange(Framebuffer::IsDepthStencilFormat(m_Spec.InternalFormat) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        imageBarrier.srcAccessMask = vk::AccessFlagBits2::eNone;
        imageBarrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;
        imageBarrier.dstStageMask = vk::PipelineStageFlagBits2::eTransfer;

        Ref<VulkanQueue> graphicsQueue = std::dynamic_pointer_cast<VulkanQueue>(m_Device->GetGraphicsQueue());

        vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
        commandBufferAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
        commandBufferAllocateInfo.commandPool = graphicsQueue->GetVKCommandPool();
        commandBufferAllocateInfo.commandBufferCount = 1;

        vk::CommandBuffer commandBuffer = m_Device->GetVKLogicalDevice().allocateCommandBuffers(commandBufferAllocateInfo)[0];
        commandBuffer.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

        vk::DependencyInfo dependencyInfo;
        dependencyInfo.sType = vk::StructureType::eDependencyInfo;
        dependencyInfo.imageMemoryBarrierCount = 1;
        dependencyInfo.pImageMemoryBarriers = &imageBarrier;
        commandBuffer.pipelineBarrier2(dependencyInfo);

        vk::BufferImageCopy region;
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = Framebuffer::IsDepthStencilFormat(m_Spec.InternalFormat) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = vk::Offset3D(0, 0, 0);
        region.imageExtent = vk::Extent3D(m_Width, m_Height, 1);
        commandBuffer.copyBufferToImage(stagingBuffer, m_Texture, vk::ImageLayout::eTransferDstOptimal, 1, &region);

        imageBarrier.oldLayout = m_TextureLayout;
        imageBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        m_TextureLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        imageBarrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
        imageBarrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;
        imageBarrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
        imageBarrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader;

        commandBuffer.pipelineBarrier2(dependencyInfo);

        commandBuffer.end();

        vk::SubmitInfo submitInfo;
        submitInfo.sType = vk::StructureType::eSubmitInfo;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = nullptr;

        graphicsQueue->GetVKQueue().submit(submitInfo, nullptr);
        graphicsQueue->GetVKQueue().waitIdle();

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

    void VulkanTexture2D::CheckFormatSupport()
    {
        switch (m_Spec.InternalFormat)
        {
        case InternalFormat::Depth24Stencil8:
            {
                vk::FormatProperties formatProperties = m_Device->GetVKPhysicalDevice().getFormatProperties(vk::Format::eD24UnormS8Uint);
                if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment))
                {
                    m_Spec.InternalFormat = InternalFormat::Depth32Stencil8;
                    CheckFormatSupport();
                }
            }
            break;

        case InternalFormat::RGB8:
            {
                vk::FormatProperties formatProperties = m_Device->GetVKPhysicalDevice().getFormatProperties(vk::Format::eR8G8B8Unorm);
                if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImage))
                {
                    m_Spec.InternalFormat = InternalFormat::RGBA8;
                    CheckFormatSupport();
                }
            }
            break;
        }
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
        case InternalFormat::Depth32Stencil8: return vk::Format::eD32SfloatS8Uint;
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
        case InternalFormat::Depth32Stencil8: return 5;
        }
        return 0;
    }

    vk::Filter VulkanTexture2D::GetVulkanFilterType(FilterType filter)
    {
        switch (filter)
        {
        case FilterType::Nearest: return vk::Filter::eNearest;
        case FilterType::Linear: return vk::Filter::eLinear;
        }
        return vk::Filter::eNearest;
    }
}
