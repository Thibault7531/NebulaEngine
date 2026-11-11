#include "OpenGL460ShaderPipeline.h"

namespace Nebula::Graphics
{
    OpenGL460ShaderPipeline::OpenGL460ShaderPipeline(const VertexLayout& layout, const PipelineSpecification& spec)
            : m_Specification{spec}
    {}

    uint32_t OpenGL460ShaderPipeline::GetUniformBlockBinding(const std::string& uniformBlockName)
    {
        return m_Shader->GetUniformBlockLocation(uniformBlockName);
    }

    uint32_t OpenGL460ShaderPipeline::GetVertexInputLocation()
    {
        return 0;
    }

    GLuint OpenGL460ShaderPipeline::GetShaderID()
    {
        return m_Shader->GetShaderID();
    }
}
