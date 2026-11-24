#pragma once

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanShaderPipeline.h"
#include "Nebula/Graphics/RenderPass.h"

namespace Nebula::Graphics
{
    class VulkanRenderPass : public RenderPass
    {
    public:
        VulkanRenderPass(Device* device, CommandBuffer* commandBuffer, Ref<Framebuffer> framebuffer, bool clear);
        ~VulkanRenderPass() override = default;

        virtual Ref<Framebuffer> GetFramebuffer() const override { return m_Framebuffer; }

        virtual RenderPass& SetCullMode(CullMode mode) override;
        virtual RenderPass& SetDepthStencilState(DepthStencilState state) override;

        virtual RenderPass& SetViewport(double x, double y, double width, double height, double near, double far) override;
        virtual RenderPass& SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        virtual RenderPass& SetShaderPipeline(Ref<ShaderPipeline> pipeline) override;
        virtual RenderPass& SetUniformBlock(const std::string& blockName, Ref<UniformBuffer> buffer, uint32_t instanceIndex) override;
        virtual RenderPass& BindTextureToSlot(Ref<Texture2D> texture, uint32_t slot) override;
        virtual RenderPass& BindTextureToSlot(Texture2D* texture, uint32_t slot) override;

        virtual RenderPass& BindVertexBuffer(Ref<VertexBuffer> vertexBuffer) override;

        virtual RenderPass& DrawIndexed(Ref<IndexBuffer> indexBuffer, uint32_t elementCount) override;
        virtual RenderPass& DrawIndexedOffset(Ref<IndexBuffer> indexBuffer, uint32_t elementCount, uint32_t indexOffset, uint32_t vertexOffset) override;

        virtual RenderPass& DrawArrayIndexed(Ref<VertexArray> vertexArray, uint32_t elementCount) override;
        virtual RenderPass& DrawArrayIndexedOffset(Ref<VertexArray> vertexArray, uint32_t elementCount, uint32_t indexOffset, uint32_t vertexOffset) override;

        virtual RenderPass& EndRenderPass() override;

    public:
        static vk::CullModeFlagBits GetVulkanCullMode(CullMode mode);
        static vk::CompareOp GetVulkanCompareFunction(CompareFunction function);
        static vk::StencilOp GetVulkanStencilOperation(StencilOperation operation);
        static vk::IndexType GetVulkanIndexType(BufferDataType type);

    private:
        VulkanDevice* m_Device;
        VulkanCommandBuffer* m_CommandBuffer;
        Ref<Framebuffer> m_Framebuffer;

        Ref<VulkanShaderPipeline> m_CurrentlyBoundShaderPipeline;
    };
}
