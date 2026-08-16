#pragma once

#include "Metal3Shader.h"
#include "Metal/Metal.hpp"
#include "Nebula/CoreSystems/Core/Core.h"
#include "Nebula/Graphics/ShaderPipeline.h"

namespace Nebula::Graphics
{
    class Metal3ShaderPipeline : public ShaderPipeline
    {
    public:
        Metal3ShaderPipeline(MTL::Device* device, Ref<Metal3Shader> shader, VertexLayout layout, PipelineSpecification spec);
        ~Metal3ShaderPipeline() override;

        virtual uint32_t GetUniformBlockLocation(const std::string& uniformBlockName) override { return m_Shader->GetUniformBlockLocation(uniformBlockName); }
        virtual uint32_t GetVertexInputLocation(const std::string& vertexInputName) override { return m_Shader->GetVertexInputLocation(vertexInputName); }

    public:
        static MTL::VertexFormat GetMetal3VertexFormat(ShaderDataType type);
        static MTL::BlendOperation GetMetal3BlendOperation(BlendOperation operation);
        static MTL::BlendFactor GetMetal3BlendFactor(BlendFactor factor);
        static bool IsColorFormatBlendable(Texture::InternalFormat format);

    public:
        MTL::RenderPipelineState* GetRenderPipelineState() { return m_RenderPipelineState; }

    private:
        Ref<Metal3Shader> m_Shader;
        PipelineSpecification m_Spec;
        MTL::RenderPipelineState* m_RenderPipelineState;
    };
}
