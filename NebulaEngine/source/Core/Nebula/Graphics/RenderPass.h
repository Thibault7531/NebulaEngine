#pragma once
#include "ShaderPipeline.h"
#include "Buffers/Buffer.h"
#include "Buffers/VertexArray.h"

namespace Nebula::Graphics
{
    enum class CullMode
    {
        None,
        Front,
        Back
    };

    enum class DepthCompareFunction
    {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };

    struct DepthStencilState
    {
        DepthCompareFunction depthCompareFunction;
        bool depthWrite;
    };

    class RenderPass
    {
    public:
        virtual ~RenderPass() = default;

        virtual Ref<Framebuffer> GetFramebuffer() const = 0;

        virtual RenderPass& SetCullMode(CullMode mode) = 0;
        virtual RenderPass& SetDepthStencilState(DepthStencilState state) = 0;

        virtual RenderPass& SetViewport(double x, double y, double width, double height, double near, double far) = 0;
        virtual RenderPass& SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual RenderPass& SetShaderPipeline(Ref<ShaderPipeline> pipeline) = 0;
        virtual RenderPass& SetUniformBlock(const std::string& blockName, Ref<UniformBuffer> buffer, uint32_t instanceIndex = 0) = 0;
        virtual RenderPass& BindTextureToSlot(Ref<Texture2D> texture, uint32_t slot = 0) = 0;
        virtual RenderPass& BindTextureToSlot(Texture2D* texture, uint32_t slot = 0) = 0;

        virtual RenderPass& BindVertexBuffer(Ref<VertexBuffer> vertexBuffer) = 0;

        virtual RenderPass& DrawIndexed(Ref<IndexBuffer> indexBuffer, uint32_t elementCount = 0) = 0;
        virtual RenderPass& DrawIndexedOffset(Ref<IndexBuffer> indexBuffer, uint32_t elementCount, uint32_t indexOffset, uint32_t vertexOffset) = 0;

        virtual RenderPass& DrawArrayIndexed(Ref<VertexArray> vertexArray, uint32_t elementCount = 0) = 0;
        virtual RenderPass& DrawArrayIndexedOffset(Ref<VertexArray> vertexArray, uint32_t elementCount, uint32_t indexOffset, uint32_t vertexOffset) = 0;
;
        virtual RenderPass& EndRenderPass() = 0;
    };
}
