#include "OpenGL460RenderPass.h"

#include "OpenGL460ShaderPipeline.h"
#include "OpenGLBuffer.h"
#include "OpenGLVertexArray.h"

namespace Nebula::Graphics
{
    OpenGL460RenderPass::OpenGL460RenderPass(Ref<Framebuffer> framebuffer, bool clear)
        : m_Framebuffer{std::dynamic_pointer_cast<OpenGL460Framebuffer>(framebuffer)}
    {
        glDisable(GL_SCISSOR_TEST);
        if (clear) m_Framebuffer->Clear();
        OpenGL460RenderPass::SetCullMode(CullMode::None);
        OpenGL460RenderPass::SetDepthStencilState(DepthStencilState{DepthCompareFunction::Less, false});
        OpenGL460RenderPass::SetViewport(0, 0, m_Framebuffer->GetSpecification().width, m_Framebuffer->GetSpecification().height, 0.f, 1.f);
        glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer->GetBufferID());
    }

    void OpenGL460RenderPass::SetCullMode(CullMode mode)
    {
        switch (mode)
        {
        case CullMode::None:
            {
                glDisable(GL_CULL_FACE);
                break;
            }
        case CullMode::Front:
            {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);
            }
        case CullMode::Back:
            {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            }
        }
    }

    void OpenGL460RenderPass::SetDepthStencilState(DepthStencilState state)
    {
        if (state.depthWrite)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GetOpenGL460DepthCompareFunction(state.depthCompareFunction));
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
        }
    }

    void OpenGL460RenderPass::SetViewport(double x, double y, double width, double height, double near, double far)
    {
        glViewport(static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), static_cast<int>(height));
        glDepthRange(near, far);

        m_Viewport = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height) };
    }

    void OpenGL460RenderPass::SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glScissor(x, m_Viewport.w - y - height, width, height);
        glEnable(GL_SCISSOR_TEST);
    }

    void OpenGL460RenderPass::SetShaderPipeline(Ref<ShaderPipeline> pipeline)
    {
        m_CurrentlyBoundShaderPipeline = std::dynamic_pointer_cast<OpenGL460ShaderPipeline>(pipeline);
        glUseProgram(m_CurrentlyBoundShaderPipeline->GetShaderID());

        PipelineSpecification spec = m_CurrentlyBoundShaderPipeline->GetSpecification();
        if (spec.blendState.enabled)
        {
            glEnable(GL_BLEND);
            glBlendEquationSeparate(GetOpenGL460BlendOperation(spec.blendState.colorOperation), GetOpenGL460BlendOperation(spec.blendState.alphaOperation));
            glBlendFuncSeparate(GetOpenGL460BlendFactor(spec.blendState.srcColorFactor), GetOpenGL460BlendFactor(spec.blendState.dstColorFactor), GetOpenGL460BlendFactor(spec.blendState.srcAlphaFactor), GetOpenGL460BlendFactor(spec.blendState.dstAlphaFactor));
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }

    void OpenGL460RenderPass::SetUniformBlock(const std::string& blockName, Ref<UniformBuffer> buffer,
        uint32_t instanceIndex)
    {
        uint32_t location = m_CurrentlyBoundShaderPipeline->GetUniformBlockLocation(blockName);
        Ref<OpenGL460UniformBuffer> uniformBuffer = std::dynamic_pointer_cast<OpenGL460UniformBuffer>(buffer);
        glBindBufferRange(GL_UNIFORM_BUFFER, location, uniformBuffer->GetBufferID(), uniformBuffer->GetLayout().GetStride() * instanceIndex, uniformBuffer->GetLayout().GetStride());
    }

    void OpenGL460RenderPass::BindTextureToSlot(Ref<Texture2D> texture, uint32_t slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, reinterpret_cast<OpenGL460Texture2D*>(texture.get())->GetTextureID());
    }

    void OpenGL460RenderPass::BindTextureToSlot(Texture2D* texture, uint32_t slot)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, reinterpret_cast<OpenGL460Texture2D*>(texture)->GetTextureID());
    }

    void OpenGL460RenderPass::BindVertexBuffer(Ref<VertexBuffer> vertexBuffer)
    {
        Ref<OpenGLVertexBuffer> vertexBufferOpenGL = std::dynamic_pointer_cast<OpenGLVertexBuffer>(vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferOpenGL->GetBufferID());
    }

    void OpenGL460RenderPass::DrawIndexed(Ref<IndexBuffer> indexBuffer, uint32_t elementCount)
    {
        DrawIndexedOffset(indexBuffer, elementCount, 0, 0);
    }

    void OpenGL460RenderPass::DrawIndexedOffset(Ref<IndexBuffer> indexBuffer, uint32_t elementCount,
        uint32_t indexOffset, uint32_t vertexOffset)
    {
        Ref<OpenGLIndexBuffer> indexBufferOpenGL = std::dynamic_pointer_cast<OpenGLIndexBuffer>(indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferOpenGL->GetBufferID());
        glDrawElements(GL_TRIANGLES, elementCount ? elementCount : indexBufferOpenGL->GetCount(), GetOpenGL460DataType(indexBufferOpenGL->GetDataType()),
            reinterpret_cast<const void*>(static_cast<intptr_t>(indexOffset)));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void OpenGL460RenderPass::DrawArrayIndexed(Ref<VertexArray> vertexArray, uint32_t elementCount)
    {
        DrawArrayIndexedOffset(vertexArray, elementCount, 0, 0);
    }

    void OpenGL460RenderPass::DrawArrayIndexedOffset(Ref<VertexArray> vertexArray, uint32_t elementCount,
        uint32_t indexOffset, uint32_t vertexOffset)
    {
        Ref<OpenGLIndexBuffer> indexBufferOpenGL = std::dynamic_pointer_cast<OpenGLIndexBuffer>(vertexArray->GetIndexBuffer());
        Ref<OpenGLVertexArray> vertexArrayOpenGL = std::dynamic_pointer_cast<OpenGLVertexArray>(vertexArray);
        glBindVertexArray(vertexArrayOpenGL->GetBufferID());
        glDrawElementsBaseVertex(GL_TRIANGLES, elementCount ? elementCount : indexBufferOpenGL->GetCount(), GetOpenGL460DataType(indexBufferOpenGL->GetDataType()),
            reinterpret_cast<const void*>(static_cast<intptr_t>(indexOffset)), vertexOffset);
        glBindVertexArray(0);
    }

    void OpenGL460RenderPass::EndRenderPass()
    {
        glDisable(GL_SCISSOR_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLint OpenGL460RenderPass::GetOpenGL460DepthCompareFunction(DepthCompareFunction function)
    {
        switch (function)
        {
        case DepthCompareFunction::Never: return GL_NEVER;
        case DepthCompareFunction::Less: return GL_LESS;
        case DepthCompareFunction::Equal: return GL_EQUAL;
        case DepthCompareFunction::LessEqual: return GL_LEQUAL;
        case DepthCompareFunction::Greater: return GL_GREATER;
        case DepthCompareFunction::NotEqual: return GL_NOTEQUAL;
        case DepthCompareFunction::GreaterEqual: return GL_GEQUAL;
        case DepthCompareFunction::Always: return GL_ALWAYS;
        }
    }

    GLenum OpenGL460RenderPass::GetOpenGL460DataType(BufferDataType type)
    {
        switch (type)
        {
        case BufferDataType::UInt16: return GL_UNSIGNED_SHORT;
        case BufferDataType::UInt32: return GL_UNSIGNED_INT;
        }
    }

    GLenum OpenGL460RenderPass::GetOpenGL460BlendOperation(BlendOperation operation)
    {
        switch (operation)
        {
        case BlendOperation::Add: return GL_FUNC_ADD;
        case BlendOperation::Subtract: return GL_FUNC_SUBTRACT;
        case BlendOperation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
        case BlendOperation::Minimum: return GL_MIN;
        case BlendOperation::Maximum: return GL_MAX;
        }
    }

    GLenum OpenGL460RenderPass::GetOpenGL460BlendFactor(BlendFactor factor)
    {
        switch (factor)
        {
        case BlendFactor::Zero: return GL_ZERO;
        case BlendFactor::One: return GL_ONE;
        case BlendFactor::SourceAlpha: return GL_SRC_ALPHA;
        case BlendFactor::OneMinusSourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DestinationAlpha: return GL_DST_ALPHA;
        case BlendFactor::OneMinusDestinationAlpha: return GL_ONE_MINUS_DST_ALPHA;
        }
    }
}
