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

        virtual void SetCullMode(CullMode mode) = 0;
        virtual void SetDepthStencilState(DepthStencilState state) = 0;

        virtual void SetViewport(double x, double y, double width, double height, double near, double far) = 0;
        virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

        virtual void SetShaderPipeline(Ref<ShaderPipeline> pipeline) = 0;
        virtual void SetUniformBlock(const std::string& blockName, Ref<UniformBuffer> buffer) = 0;
        virtual void BindTextureToSlot(Ref<Texture2D> texture, uint32_t slot = 0) = 0;
        virtual void BindTextureToSlot(Texture2D* texture, uint32_t slot = 0) = 0;

        virtual void BindVertexBuffer(Ref<VertexBuffer> vertexBuffer) = 0;

        virtual void DrawIndexed(Ref<IndexBuffer> indexBuffer, uint32_t elementCount = 0) = 0;
        virtual void DrawIndexedOffset(Ref<IndexBuffer> indexBuffer, uint32_t elementCount, uint32_t indexOffset, uint32_t vertexOffset) = 0;

        virtual void DrawArrayIndexed(Ref<VertexArray> vertexArray, uint32_t elementCount = 0) = 0;
        virtual void DrawArrayIndexedOffset(Ref<VertexArray> vertexArray, uint32_t elementCount, uint32_t indexOffset, uint32_t vertexOffset) = 0;
;
        virtual void EndRenderPass() = 0;
    };
}
