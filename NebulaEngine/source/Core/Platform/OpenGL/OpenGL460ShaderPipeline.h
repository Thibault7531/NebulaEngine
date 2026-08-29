#pragma once
#include "OpenGLShader.h"
#include "Nebula/Graphics/ShaderPipeline.h"

namespace Nebula::Graphics
{
    class OpenGL460ShaderPipeline : public ShaderPipeline
    {
    public:
        OpenGL460ShaderPipeline(Ref<OpenGLShader> shader, VertexLayout layout, const PipelineSpecification& spec);
        ~OpenGL460ShaderPipeline() = default;

        virtual uint32_t GetUniformBlockLocation(const std::string& uniformBlockName) override;
        virtual uint32_t GetVertexInputLocation(const std::string& vertexInputName) override;

    public:
        GLuint GetShaderID();
        const PipelineSpecification& GetSpecification() const { return m_Specification; }

    private:
        Ref<OpenGLShader> m_Shader;
        PipelineSpecification m_Specification;
    };
}
