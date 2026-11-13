#include "VulkanRenderPass.h"

#include "VulkanBuffer.h"
#include "VulkanShaderPipeline.h"
#include "VulkanTexture.h"

#undef near
#undef far

namespace Nebula::Graphics
{
    VulkanRenderPass::VulkanRenderPass(Device* device, CommandBuffer* commandBuffer, Ref<Framebuffer> framebuffer, bool clear)
        : m_Device{dynamic_cast<VulkanDevice*>(device)}, m_CommandBuffer{dynamic_cast<VulkanCommandBuffer*>(commandBuffer)}, m_Framebuffer{framebuffer}
    {
        std::vector<vk::ImageMemoryBarrier2> imageBarriers;
        std::vector<vk::RenderingAttachmentInfo> colorAttachmentInfos;
        for (uint32_t colorAttachmentIndex = 0; colorAttachmentIndex < framebuffer->GetColorAttachmentCount(); colorAttachmentIndex++)
        {
            Ref<VulkanTexture2D> colorAttachment = std::dynamic_pointer_cast<VulkanTexture2D>(framebuffer->GetColorAttachment(colorAttachmentIndex));
            vk::Image colorAttachmentImage = colorAttachment->GetVKImage();
            vk::ImageMemoryBarrier2 imageBarrier;
            imageBarrier.sType = vk::StructureType::eImageMemoryBarrier2;
            imageBarrier.image = colorAttachmentImage;
            imageBarrier.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
            imageBarrier.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            imageBarrier.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            imageBarrier.srcAccessMask = vk::AccessFlagBits2::eNone;
            imageBarrier.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
            imageBarrier.oldLayout = colorAttachment->GetVKImageLayout();
            imageBarrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
            colorAttachment->SetVKImageLayout(vk::ImageLayout::eColorAttachmentOptimal);
            imageBarriers.push_back(imageBarrier);

            vk::RenderingAttachmentInfo colorAttachmentInfo;
            colorAttachmentInfo.sType = vk::StructureType::eRenderingAttachmentInfo;
            colorAttachmentInfo.imageView = dynamic_cast<VulkanTexture2D*>(colorAttachment.get())->GetVKImageView();
            colorAttachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
            colorAttachmentInfo.loadOp = clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
            colorAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
            colorAttachmentInfo.clearValue = vk::ClearColorValue(std::array<float, 4>{framebuffer->GetClearColors()[colorAttachmentIndex].r, framebuffer->GetClearColors()[colorAttachmentIndex].g, framebuffer->GetClearColors()[colorAttachmentIndex].b, framebuffer->GetClearColors()[colorAttachmentIndex].a});
            colorAttachmentInfos.push_back(colorAttachmentInfo);
        }
        if (framebuffer->GetDepthAttachment())
        {
            Ref<VulkanTexture2D> depthAttachment = std::dynamic_pointer_cast<VulkanTexture2D>(framebuffer->GetDepthAttachment());
            vk::Image depthAttachmentImage = depthAttachment->GetVKImage();
            vk::ImageMemoryBarrier2 imageBarrier;
            imageBarrier.sType = vk::StructureType::eImageMemoryBarrier2;
            imageBarrier.image = depthAttachmentImage;
            imageBarrier.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1);
            imageBarrier.srcStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests;
            imageBarrier.dstStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests;
            imageBarrier.srcAccessMask = vk::AccessFlagBits2::eNone;
            imageBarrier.dstAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentWrite;
            imageBarrier.oldLayout = depthAttachment->GetVKImageLayout();
            imageBarrier.newLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            depthAttachment->SetVKImageLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
            imageBarriers.push_back(imageBarrier);
        }

        vk::DependencyInfo dependencyInfo;
        dependencyInfo.sType = vk::StructureType::eDependencyInfo;
        dependencyInfo.imageMemoryBarrierCount = static_cast<uint32_t>(imageBarriers.size());
        dependencyInfo.pImageMemoryBarriers = imageBarriers.data();
        m_CommandBuffer->GetVKCommandBuffer().pipelineBarrier2(dependencyInfo);

        vk::RenderingInfo renderingInfo;
        renderingInfo.sType = vk::StructureType::eRenderingInfo;
        renderingInfo.renderArea = vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(framebuffer->GetSpecification().width, framebuffer->GetSpecification().height));
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentInfos.size());
        renderingInfo.pColorAttachments = colorAttachmentInfos.data();
        if (framebuffer->GetDepthAttachment())
        {
            vk::RenderingAttachmentInfo depthAttachmentInfo;
            depthAttachmentInfo.sType = vk::StructureType::eRenderingAttachmentInfo;
            depthAttachmentInfo.imageView = dynamic_cast<VulkanTexture2D*>(framebuffer->GetDepthAttachment().get())->GetVKImageView();
            depthAttachmentInfo.imageLayout = vk::ImageLayout::eDepthAttachmentOptimal;
            depthAttachmentInfo.loadOp = clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
            depthAttachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
            depthAttachmentInfo.clearValue = vk::ClearDepthStencilValue{framebuffer->GetClearDepth(), 0};
            renderingInfo.pDepthAttachment = &depthAttachmentInfo;
        }
        m_CommandBuffer->GetVKCommandBuffer().beginRendering(renderingInfo);

        VulkanRenderPass::SetCullMode(CullMode::None);
        VulkanRenderPass::SetDepthStencilState(DepthStencilState{false, false, CompareFunction::Greater, false, 0xFF, 0xFF, CompareFunction::Always, StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Keep});
        VulkanRenderPass::SetViewport(0, 0, framebuffer->GetSpecification().width, framebuffer->GetSpecification().height, 0.f, 1.f);
        VulkanRenderPass::SetScissor(0, 0, framebuffer->GetSpecification().width, framebuffer->GetSpecification().height);
    }

    RenderPass& VulkanRenderPass::SetCullMode(CullMode mode)
    {
        m_CommandBuffer->GetVKCommandBuffer().setCullMode(GetVulkanCullMode(mode));
        return *this;
    }

    RenderPass& VulkanRenderPass::SetDepthStencilState(DepthStencilState state)
    {
        m_CommandBuffer->GetVKCommandBuffer().setDepthTestEnable(state.depthTest);
        m_CommandBuffer->GetVKCommandBuffer().setDepthWriteEnable(state.depthWrite);
        m_CommandBuffer->GetVKCommandBuffer().setDepthCompareOp(GetVulkanCompareFunction(state.depthCompareFunction));

        m_CommandBuffer->GetVKCommandBuffer().setStencilTestEnable(state.stencilTest);
        m_CommandBuffer->GetVKCommandBuffer().setStencilWriteMask(vk::StencilFaceFlagBits::eFrontAndBack, state.stencilWriteMask);
        m_CommandBuffer->GetVKCommandBuffer().setStencilCompareMask(vk::StencilFaceFlagBits::eFrontAndBack, state.stencilCompareMask);
        m_CommandBuffer->GetVKCommandBuffer().setStencilOp(vk::StencilFaceFlagBits::eFrontAndBack, GetVulkanStencilOperation(state.stencilFailOperation), GetVulkanStencilOperation(state.stencilPassOperation), GetVulkanStencilOperation(state.stencilDepthFailOperation), GetVulkanCompareFunction(state.stencilCompareFunction));
        return *this;
    }

    RenderPass& VulkanRenderPass::
    SetViewport(double x, double y, double width, double height, double near, double far) {
        vk::Viewport viewport;
        viewport.x = static_cast<float>(x);
        viewport.y = static_cast<float>(y + height);
        viewport.width = static_cast<float>(width);
        viewport.height = -static_cast<float>(height);
        viewport.minDepth = static_cast<float>(near);
        viewport.maxDepth = static_cast<float>(far);
        m_CommandBuffer->GetVKCommandBuffer().setViewport(0, 1, &viewport);
        return *this;
    }

    RenderPass& VulkanRenderPass::SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        vk::Rect2D scissorRect;
        scissorRect.offset = vk::Offset2D(static_cast<int32_t>(x), static_cast<int32_t>(y));
        scissorRect.extent = vk::Extent2D(width, height);
        m_CommandBuffer->GetVKCommandBuffer().setScissor(0, 1, &scissorRect);
        return *this;
    }

    RenderPass& VulkanRenderPass::SetShaderPipeline(Ref<ShaderPipeline> pipeline)
    {
        Ref<VulkanShaderPipeline> vulkanPipeline = std::dynamic_pointer_cast<VulkanShaderPipeline>(pipeline);
        m_CommandBuffer->GetVKCommandBuffer().bindPipeline(vk::PipelineBindPoint::eGraphics, vulkanPipeline->GetVKPipeline());
        m_CurrentlyBoundShaderPipeline = vulkanPipeline;
        return *this;
    }

    RenderPass& VulkanRenderPass::SetUniformBlock(const std::string& blockName, Ref<UniformBuffer> buffer, uint32_t instanceIndex)
    {
        Ref<VulkanUniformBuffer> vulkanBuffer = std::dynamic_pointer_cast<VulkanUniformBuffer>(buffer);

        vk::DescriptorBufferInfo descriptorBufferInfo;
        descriptorBufferInfo.buffer = vulkanBuffer->GetVKBuffer();
        descriptorBufferInfo.offset = buffer->GetLayout().GetStride() * instanceIndex;
        descriptorBufferInfo.range = buffer->GetLayout().GetStride();

        vk::WriteDescriptorSet writeDescriptorSet;
        writeDescriptorSet.sType = vk::StructureType::eWriteDescriptorSet;
        writeDescriptorSet.dstBinding = m_CurrentlyBoundShaderPipeline->GetUniformBlockBinding(blockName);
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = vk::DescriptorType::eUniformBuffer;
        writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

        m_CommandBuffer->GetVKCommandBuffer().pushDescriptorSetKHR(vk::PipelineBindPoint::eGraphics, m_CurrentlyBoundShaderPipeline->GetVKPipelineLayout(), m_CurrentlyBoundShaderPipeline->GetUniformBlockSet(blockName), 1, &writeDescriptorSet);

        return *this;
    }

    RenderPass& VulkanRenderPass::BindTextureToSlot(Ref<Texture2D> texture, uint32_t slot)
    {
        Ref<VulkanTexture2D> vulkanTexture = std::dynamic_pointer_cast<VulkanTexture2D>(texture);
        const ResourceBinding textureBinding =
            m_CurrentlyBoundShaderPipeline->GetResourceBinding(ResourceType::Texture);

        vk::DescriptorImageInfo descriptorImageInfo;
        descriptorImageInfo.imageView = vulkanTexture->GetVKImageView();
        descriptorImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        descriptorImageInfo.sampler = vulkanTexture->GetVKSampler();

        vk::WriteDescriptorSet writeDescriptorSet;
        writeDescriptorSet.sType = vk::StructureType::eWriteDescriptorSet;
        writeDescriptorSet.dstBinding = textureBinding.Binding;
        writeDescriptorSet.dstArrayElement = textureBinding.DescriptorCount > 1 ? slot : 0;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        writeDescriptorSet.pImageInfo = &descriptorImageInfo;

        m_CommandBuffer->GetVKCommandBuffer().pushDescriptorSetKHR(vk::PipelineBindPoint::eGraphics, m_CurrentlyBoundShaderPipeline->GetVKPipelineLayout(), textureBinding.Set, 1, &writeDescriptorSet);

        return *this;
    }

    RenderPass& VulkanRenderPass::BindTextureToSlot(Texture2D* texture, uint32_t slot)
    {
        auto* vulkanTexture = dynamic_cast<VulkanTexture2D*>(texture);
        const ResourceBinding textureBinding =
            m_CurrentlyBoundShaderPipeline->GetResourceBinding(ResourceType::Texture);

        vk::DescriptorImageInfo descriptorImageInfo;
        descriptorImageInfo.imageView = vulkanTexture->GetVKImageView();
        descriptorImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        descriptorImageInfo.sampler = vulkanTexture->GetVKSampler();

        vk::WriteDescriptorSet writeDescriptorSet;
        writeDescriptorSet.sType = vk::StructureType::eWriteDescriptorSet;
        writeDescriptorSet.dstBinding = textureBinding.Binding;
        writeDescriptorSet.dstArrayElement = textureBinding.DescriptorCount > 1 ? slot : 0;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        writeDescriptorSet.pImageInfo = &descriptorImageInfo;

        m_CommandBuffer->GetVKCommandBuffer().pushDescriptorSetKHR(vk::PipelineBindPoint::eGraphics, m_CurrentlyBoundShaderPipeline->GetVKPipelineLayout(), textureBinding.Set, 1, &writeDescriptorSet);

        return *this;
    }

    RenderPass& VulkanRenderPass::BindVertexBuffer(Ref<VertexBuffer> vertexBuffer)
    {
        Ref<VulkanVertexBuffer> vulkanVertexBuffer = std::dynamic_pointer_cast<VulkanVertexBuffer>(vertexBuffer);
        vk::Buffer vertexBuffers[] = { vulkanVertexBuffer->GetVKBuffer() };
        vk::DeviceSize offsets[] = { 0 };
        m_CommandBuffer->GetVKCommandBuffer().bindVertexBuffers(0, 1, vertexBuffers, offsets);
        return *this;
    }

    RenderPass& VulkanRenderPass::DrawIndexed(Ref<IndexBuffer> indexBuffer, uint32_t elementCount)
    {
        return DrawIndexedOffset(indexBuffer, elementCount == 0 ? indexBuffer->GetCount() : elementCount, 0, 0);
    }

    RenderPass& VulkanRenderPass::DrawIndexedOffset(Ref<IndexBuffer> indexBuffer, uint32_t elementCount, uint32_t indexOffset, uint32_t vertexOffset)
    {
        Ref<VulkanIndexBuffer> vulkanIndexBuffer = std::dynamic_pointer_cast<VulkanIndexBuffer>(indexBuffer);
        m_CommandBuffer->GetVKCommandBuffer().bindIndexBuffer(vulkanIndexBuffer->GetVKBuffer(), 0, GetVulkanIndexType(indexBuffer->GetDataType()));
        m_CommandBuffer->GetVKCommandBuffer().drawIndexed(elementCount == 0 ? indexBuffer->GetCount() : elementCount, 1, indexOffset, vertexOffset, 0);
        return *this;
    }

    RenderPass& VulkanRenderPass::DrawArrayIndexed(Ref<VertexArray> vertexArray, uint32_t elementCount)
    {
        return DrawArrayIndexedOffset(vertexArray, elementCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : elementCount, 0, 0);
    }

    RenderPass& VulkanRenderPass::DrawArrayIndexedOffset(Ref<VertexArray> vertexArray, uint32_t elementCount, uint32_t indexOffset, uint32_t vertexOffset)
    {
        BindVertexBuffer(vertexArray->GetVertexBuffers()[0]);
        DrawIndexedOffset(vertexArray->GetIndexBuffer(), elementCount, indexOffset, vertexOffset);
        return *this;
    }

    RenderPass& VulkanRenderPass::EndRenderPass()
    {
        m_CommandBuffer->GetVKCommandBuffer().endRendering();

        uint32_t colorAttachmentCount = m_Framebuffer->GetColorAttachmentCount();
        std::vector<vk::ImageMemoryBarrier2> imageBarriers;
        for (uint32_t colorAttachmentIndex = 0; colorAttachmentIndex < colorAttachmentCount; colorAttachmentIndex++)
        {
            Ref<VulkanTexture2D> colorAttachment = std::dynamic_pointer_cast<VulkanTexture2D>(m_Framebuffer->GetColorAttachment(colorAttachmentIndex));
            if (colorAttachment == m_Framebuffer->GetSwapchainColorAttachment())
            {
                vk::ImageMemoryBarrier2 imagePresentBarrier;
                imagePresentBarrier.sType = vk::StructureType::eImageMemoryBarrier2;
                imagePresentBarrier.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
                imagePresentBarrier.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
                imagePresentBarrier.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
                imagePresentBarrier.dstAccessMask = vk::AccessFlagBits2::eNone;
                imagePresentBarrier.oldLayout = colorAttachment->GetVKImageLayout();
                imagePresentBarrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
                colorAttachment->SetVKImageLayout(vk::ImageLayout::ePresentSrcKHR);
                imagePresentBarrier.image = colorAttachment->GetVKImage();
                imagePresentBarrier.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
                imageBarriers.push_back(imagePresentBarrier);
            }
            else
            {
                vk::ImageMemoryBarrier2 imageBarrier;
                imageBarrier.sType = vk::StructureType::eImageMemoryBarrier2;
                imageBarrier.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
                imageBarrier.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
                imageBarrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader;
                imageBarrier.dstAccessMask = vk::AccessFlagBits2::eNone;
                imageBarrier.oldLayout = colorAttachment->GetVKImageLayout();
                imageBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
                colorAttachment->SetVKImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
                imageBarrier.image = colorAttachment->GetVKImage();
                imageBarrier.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
                imageBarriers.push_back(imageBarrier);
            }
        }

        vk::DependencyInfo imagePresentDependencyInfo;
        imagePresentDependencyInfo.sType = vk::StructureType::eDependencyInfo;
        imagePresentDependencyInfo.imageMemoryBarrierCount = imageBarriers.size();
        imagePresentDependencyInfo.pImageMemoryBarriers = imageBarriers.data();
        m_CommandBuffer->GetVKCommandBuffer().pipelineBarrier2(imagePresentDependencyInfo);

        return *this;
    }

    vk::CullModeFlagBits VulkanRenderPass::GetVulkanCullMode(CullMode mode)
    {
        switch (mode)
        {
            case CullMode::None: return vk::CullModeFlagBits::eNone;
            case CullMode::Front: return vk::CullModeFlagBits::eFront;
            case CullMode::Back: return vk::CullModeFlagBits::eBack;
            default: return vk::CullModeFlagBits::eNone;
        }
    }

    vk::CompareOp VulkanRenderPass::GetVulkanCompareFunction(CompareFunction function)
    {
        switch (function)
        {
            case CompareFunction::Never: return vk::CompareOp::eNever;
            case CompareFunction::Less: return vk::CompareOp::eLess;
            case CompareFunction::Equal: return vk::CompareOp::eEqual;
            case CompareFunction::LessEqual: return vk::CompareOp::eLessOrEqual;
            case CompareFunction::Greater: return vk::CompareOp::eGreater;
            case CompareFunction::NotEqual: return vk::CompareOp::eNotEqual;
            case CompareFunction::GreaterEqual: return vk::CompareOp::eGreaterOrEqual;
            case CompareFunction::Always: return vk::CompareOp::eAlways;
            default: return vk::CompareOp::eAlways;
        }
    }

    vk::StencilOp VulkanRenderPass::GetVulkanStencilOperation(StencilOperation operation)
    {
        switch (operation)
        {
        case StencilOperation::Keep: return vk::StencilOp::eKeep;
        case StencilOperation::Zero: return vk::StencilOp::eZero;
        case StencilOperation::Replace: return vk::StencilOp::eReplace;
        case StencilOperation::Increment: return vk::StencilOp::eIncrementAndClamp;
        case StencilOperation::IncrementWrap: return vk::StencilOp::eIncrementAndWrap;
        case StencilOperation::Decrement: return vk::StencilOp::eDecrementAndClamp;
        case StencilOperation::DecrementWrap: return vk::StencilOp::eDecrementAndWrap;
        case StencilOperation::Invert: return vk::StencilOp::eInvert;
        }
    }

    vk::IndexType VulkanRenderPass::GetVulkanIndexType(BufferDataType type)
    {
        switch (type)
        {
        case BufferDataType::UInt16: return vk::IndexType::eUint16;
        case BufferDataType::UInt32: return vk::IndexType::eUint32;
        default: return vk::IndexType::eUint32;
        }
    }
}
