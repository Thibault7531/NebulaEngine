#pragma once

#include "Metal3Framebuffer.h"
#include "Metal3ShaderPipeline.h"
#include "Metal/Metal.hpp"
#include "Nebula/CoreSystems/Core/Core.h"
#include "Nebula/Graphics/RenderPass.h"

namespace Nebula::Graphics
{
    class Metal3RenderPass : public RenderPass
    {
    public:
        Metal3RenderPass(MTL::Device* device, MTL::CommandBuffer* commandBuffer, Ref<Metal3Framebuffer> framebuffer, bool clear);
        ~Metal3RenderPass() override;

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
        MTL::CullMode GetMetal3CullMode(CullMode mode);
        MTL::CompareFunction GetMetal3CompareFunction(DepthCompareFunction function);

    private:
        MTL::Device* m_Device;
        MTL::RenderCommandEncoder* m_RenderCommandEncoder;

        Ref<Metal3Framebuffer> m_Framebuffer;
        Ref<Metal3ShaderPipeline> m_CurrentlyBoundShaderPipeline;
        MTL::DepthStencilState* m_CurrentDepthStencilState;
    };
}
