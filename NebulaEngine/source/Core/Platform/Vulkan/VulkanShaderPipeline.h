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

        virtual ShaderStage GetUniformBlockShaderStage(const std::string& uniformBlockName) override;
        virtual uint32_t GetUniformBlockBinding(const std::string& uniformBlockName) override;
        virtual uint32_t GetUniformBlockSet(const std::string& uniformBlockName) override;
        virtual ResourceBinding GetUniformBlockResourceBinding(const std::string& uniformBlockName) override;
        virtual uint32_t GetVertexInputLocation() override { return 0; }

    public:
        vk::PipelineLayout GetVKPipelineLayout() const { return m_PipelineLayout; }
        vk::Pipeline GetVKPipeline() const { return m_Pipeline; }
        vk::DescriptorType GetVKDescriptorType(ResourceType type);

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
