#pragma once

#undef near
#undef far

#include "OpenGL460Framebuffer.h"
#include "OpenGL460ShaderPipeline.h"
#include "Nebula/Graphics/RenderPass.h"

namespace Nebula::Graphics
{
    class OpenGL460RenderPass : public RenderPass
    {
    public:
        OpenGL460RenderPass(Ref<Framebuffer> framebuffer, bool clear);
        virtual ~OpenGL460RenderPass() = default;

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

    private:
        GLint GetOpenGL460DepthCompareFunction(DepthCompareFunction function);
        GLenum GetOpenGL460DataType(BufferDataType type);
        static GLenum GetOpenGL460BlendOperation(BlendOperation operation);
        static GLenum GetOpenGL460BlendFactor(BlendFactor factor);

    private:
        Ref<OpenGL460Framebuffer> m_Framebuffer;
        Ref<OpenGL460ShaderPipeline> m_CurrentlyBoundShaderPipeline;

        glm::vec4 m_Viewport;
    };
}
