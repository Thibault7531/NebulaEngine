#include "Metal3RenderPass.h"

#include "Metal3Buffer.h"

namespace Nebula::Graphics
{
    Metal3RenderPass::Metal3RenderPass(MTL::Device* device, MTL::CommandBuffer* commandBuffer, Ref<Metal3Framebuffer> framebuffer, bool clear)
        : m_Device{device}, m_Framebuffer{framebuffer}
    {
        MTL::RenderPassDescriptor* renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();

        for (uint32_t attachment = 0; attachment < framebuffer->GetColorAttachmentSpecs().size(); attachment++)
        {
            glm::vec4 clearColor = framebuffer->GetClearColors()[attachment];
            Ref<Metal3Texture2D> texture = std::dynamic_pointer_cast<Metal3Texture2D>(framebuffer->GetColorAttachment(attachment));
            MTL::RenderPassColorAttachmentDescriptor* colorAttachmentDescriptor = renderPassDescriptor->colorAttachments()->object(attachment);
            if (clear) colorAttachmentDescriptor->setLoadAction(MTL::LoadActionClear);
            else colorAttachmentDescriptor->setLoadAction(MTL::LoadActionLoad);
            colorAttachmentDescriptor->setStoreAction(MTL::StoreActionStore);
            colorAttachmentDescriptor->setClearColor(MTL::ClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a));
            colorAttachmentDescriptor->setTexture(texture->GetMetalTexture());
        }

        Ref<Metal3Texture2D> depth = std::dynamic_pointer_cast<Metal3Texture2D>(framebuffer->GetDepthAttachment());
        if (depth)
        {
            if (clear) renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
            else renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionLoad);
            renderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionStore);
            renderPassDescriptor->depthAttachment()->setClearDepth(framebuffer->GetClearDepth());
            renderPassDescriptor->depthAttachment()->setTexture(depth->GetMetalTexture());
        }

        m_RenderCommandEncoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);

        renderPassDescriptor->release();
    }

    Metal3RenderPass::~Metal3RenderPass()
    {
        if (m_CurrentDepthStencilState) m_CurrentDepthStencilState->release();
    }

    void Metal3RenderPass::SetCullMode(CullMode mode)
    {
        m_RenderCommandEncoder->setCullMode(GetMetal3CullMode(mode));
    }

    void Metal3RenderPass::SetDepthStencilState(DepthStencilState state)
    {
        if (m_CurrentDepthStencilState) m_CurrentDepthStencilState->release();
        MTL::DepthStencilDescriptor* descriptor = MTL::DepthStencilDescriptor::alloc()->init();
        descriptor->setDepthCompareFunction(GetMetal3CompareFunction(state.depthCompareFunction));
        descriptor->setDepthWriteEnabled(state.depthWrite);
        m_CurrentDepthStencilState = m_Device->newDepthStencilState(descriptor);
        m_RenderCommandEncoder->setDepthStencilState(m_CurrentDepthStencilState);
        descriptor->release();
    }

    void Metal3RenderPass::SetViewport(double x, double y, double width, double height, double near, double far)
    {
        m_RenderCommandEncoder->setViewport(MTL::Viewport(x, y, width, height, near, far));
    }

    void Metal3RenderPass::SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        m_RenderCommandEncoder->setScissorRect(MTL::ScissorRect(x, y, width, height));
    }

    void Metal3RenderPass::SetShaderPipeline(Ref<ShaderPipeline> pipeline)
    {
        m_CurrentlyBoundShaderPipeline = std::dynamic_pointer_cast<Metal3ShaderPipeline>(pipeline);
        MTL::RenderPipelineState* nativePipeline = m_CurrentlyBoundShaderPipeline->GetRenderPipelineState();
        m_RenderCommandEncoder->setRenderPipelineState(nativePipeline);
    }

    void Metal3RenderPass::SetUniformBlock(const std::string& blockName, Ref<UniformBuffer> buffer,
        uint32_t instanceIndex)
    {
        uint32_t location = m_CurrentlyBoundShaderPipeline->GetUniformBlockLocation(blockName);
        Ref<Metal3UniformBuffer> uniformBuffer = std::dynamic_pointer_cast<Metal3UniformBuffer>(buffer);
        m_RenderCommandEncoder->setVertexBuffer(uniformBuffer->GetBuffer(), uniformBuffer->GetLayout().GetStride() * instanceIndex, location);
        m_RenderCommandEncoder->setFragmentBuffer(uniformBuffer->GetBuffer(), uniformBuffer->GetLayout().GetStride() * instanceIndex, location);
    }

    void Metal3RenderPass::BindTextureToSlot(Ref<Texture2D> texture, uint32_t slot)
    {
        Ref<Metal3Texture2D> metalTexture = std::dynamic_pointer_cast<Metal3Texture2D>(texture);
        m_RenderCommandEncoder->setFragmentTexture(metalTexture->GetMetalTexture(), slot);
        m_RenderCommandEncoder->setFragmentSamplerState(metalTexture->GetMetalSampler(), slot);
    }

    void Metal3RenderPass::BindTextureToSlot(Texture2D* texture, uint32_t slot)
    {
        auto* metalTexture = reinterpret_cast<Metal3Texture2D*>(texture);
        m_RenderCommandEncoder->setFragmentTexture(metalTexture->GetMetalTexture(), slot);
        m_RenderCommandEncoder->setFragmentSamplerState(metalTexture->GetMetalSampler(), slot);
    }

    void Metal3RenderPass::BindVertexBuffer(Ref<VertexBuffer> vertexBuffer)
    {
        Ref<Metal3VertexBuffer> metalBuffer = std::dynamic_pointer_cast<Metal3VertexBuffer>(vertexBuffer);
        m_RenderCommandEncoder->setVertexBuffer(metalBuffer->GetBuffer(), 0, m_CurrentlyBoundShaderPipeline->GetVertexInputLocation());
    }

    void Metal3RenderPass::DrawIndexed(Ref<IndexBuffer> indexBuffer, uint32_t elementCount)
    {
        Ref<Metal3IndexBuffer> metalIndexBuffer = std::dynamic_pointer_cast<Metal3IndexBuffer>(indexBuffer);
        if (elementCount == 0) elementCount = metalIndexBuffer->GetCount();
        if (elementCount) m_RenderCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, elementCount, Metal3IndexBuffer::GetIndexType(metalIndexBuffer->GetDataType()), metalIndexBuffer->GetBuffer(), 0, 1, 0, 0);
    }

    void Metal3RenderPass::DrawIndexedOffset(Ref<IndexBuffer> indexBuffer, uint32_t elementCount, uint32_t indexOffset, uint32_t vertexOffset)
    {
        Ref<Metal3IndexBuffer> metalIndexBuffer = std::dynamic_pointer_cast<Metal3IndexBuffer>(indexBuffer);
        if (elementCount) m_RenderCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, elementCount, Metal3IndexBuffer::GetIndexType(metalIndexBuffer->GetDataType()), metalIndexBuffer->GetBuffer(), indexOffset, 1, vertexOffset, 0);
    }

    void Metal3RenderPass::DrawArrayIndexed(Ref<VertexArray> vertexArray, uint32_t elementCount)
    {
        BindVertexBuffer(vertexArray->GetVertexBuffers()[0]);
        DrawIndexed(vertexArray->GetIndexBuffer(), elementCount);
    }

    void Metal3RenderPass::DrawArrayIndexedOffset(Ref<VertexArray> vertexArray, uint32_t elementCount, uint32_t indexOffset, uint32_t vertexOffset)
    {
        BindVertexBuffer(vertexArray->GetVertexBuffers()[0]);
        DrawIndexedOffset(vertexArray->GetIndexBuffer(), elementCount, indexOffset, vertexOffset);
    }

    void Metal3RenderPass::EndRenderPass()
    {
        m_RenderCommandEncoder->endEncoding();
        m_CurrentlyBoundShaderPipeline.reset();
    }

    MTL::CullMode Metal3RenderPass::GetMetal3CullMode(CullMode mode)
    {
        switch (mode)
        {
        case CullMode::None: return MTL::CullModeNone;
        case CullMode::Front: return MTL::CullModeFront;
        case CullMode::Back: return MTL::CullModeBack;
        }
    }

    MTL::CompareFunction Metal3RenderPass::GetMetal3CompareFunction(DepthCompareFunction function)
    {
        switch (function)
        {
        case DepthCompareFunction::Never: return MTL::CompareFunctionNever;
        case DepthCompareFunction::Less: return MTL::CompareFunctionLess;
        case DepthCompareFunction::Equal: return MTL::CompareFunctionEqual;
        case DepthCompareFunction::LessEqual: return MTL::CompareFunctionLessEqual;
        case DepthCompareFunction::Greater: return MTL::CompareFunctionGreater;
        case DepthCompareFunction::NotEqual: return MTL::CompareFunctionNotEqual;
        case DepthCompareFunction::GreaterEqual: return MTL::CompareFunctionGreaterEqual;
        case DepthCompareFunction::Always: return MTL::CompareFunctionAlways;
        }
    }
}
