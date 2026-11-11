#pragma once
#include "OpenGLShader.h"
#include "Nebula/Graphics/ShaderPipeline.h"

namespace Nebula::Graphics
{
    class OpenGL460ShaderPipeline : public ShaderPipeline
    {
    public:
        OpenGL460ShaderPipeline(const VertexLayout& layout, const PipelineSpecification& spec);
        ~OpenGL460ShaderPipeline() = default;

        virtual ShaderStage GetUniformBlockShaderStage(const std::string& uniformBlockName) override { return ShaderStage::Vertex; }
        virtual uint32_t GetUniformBlockBinding(const std::string& uniformBlockName) override;
        virtual uint32_t GetUniformBlockSet(const std::string& uniformBlockName) override { return 0; }
        virtual ResourceBinding GetUniformBlockResourceBinding(const std::string& uniformBlockName) override { return ResourceBinding{}; }
        virtual uint32_t GetVertexInputLocation() override;

    public:
        GLuint GetShaderID();
        const PipelineSpecification& GetSpecification() const { return m_Specification; }

    private:
        Ref<OpenGLShader> m_Shader;
        PipelineSpecification m_Specification;
    };
}
