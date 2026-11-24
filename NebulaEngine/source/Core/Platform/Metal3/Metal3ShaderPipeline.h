#pragma once

#include "Metal3ShaderModule.h"
#include "Metal/Metal.hpp"
#include "Nebula/CoreSystems/Core/Core.h"
#include "Nebula/Graphics/ShaderPipeline.h"

namespace Nebula::Graphics
{
    class Metal3ShaderPipeline : public ShaderPipeline
    {
    public:
        Metal3ShaderPipeline(MTL::Device* device, const VertexLayout& layout, const PipelineSpecification& spec);
        ~Metal3ShaderPipeline() override;

        virtual ShaderStage GetUniformBlockShaderStage(const std::string& uniformBlockName) override;
        virtual uint32_t GetUniformBlockBinding(const std::string& uniformBlockName) override;
        virtual uint32_t GetUniformBlockSet(const std::string& uniformBlockName) override;
        virtual ResourceBinding GetUniformBlockResourceBinding(const std::string& uniformBlockName) override;
        virtual uint32_t GetVertexInputLocation() override;

    public:
        static MTL::VertexFormat GetMetal3VertexFormat(ShaderDataType type);
        static MTL::BlendOperation GetMetal3BlendOperation(BlendOperation operation);
        static MTL::BlendFactor GetMetal3BlendFactor(BlendFactor factor);
        static bool IsColorFormatBlendable(Texture::InternalFormat format);

    public:
        MTL::RenderPipelineState* GetRenderPipelineState() { return m_RenderPipelineState; }

    private:
        VertexLayout m_VertexLayout;
        PipelineSpecification m_Specification;

        MTL::RenderPipelineState* m_RenderPipelineState;
    };
}
