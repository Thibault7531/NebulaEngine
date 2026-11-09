#pragma once

#include "VulkanDevice.h"
#include "Nebula/Graphics/ShaderPipeline.h"

namespace Nebula::Graphics
{
    class VulkanShaderPipeline : public ShaderPipeline
    {
    public:
        VulkanShaderPipeline(Device* device, const VertexLayout& layout, const PipelineSpecification& spec);
        ~VulkanShaderPipeline() override;

        virtual uint32_t GetUniformBlockLocation(const std::string& uniformBlockName) override;
        virtual uint32_t GetVertexInputLocation() override { return 0; }

    public:
        static vk::Format GetVulkanVertexFormat(ShaderDataType type);
        static vk::ShaderStageFlagBits GetVulkanShaderStage(ShaderStage stage);
        static vk::BlendFactor GetVulkanBlendFactor(BlendFactor factor);
        static vk::BlendOp GetVulkanBlendOperation(BlendOperation operation);

    private:
        VulkanDevice* m_Device;
        VertexLayout m_VertexLayout;
        PipelineSpecification m_Specification;

        vk::PipelineLayout m_PipelineLayout;
        vk::Pipeline m_Pipeline;
    };
}
