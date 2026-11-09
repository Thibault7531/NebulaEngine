#include "VulkanShaderPipeline.h"

#include "VulkanShaderModule.h"
#include "VulkanTexture.h"

Nebula::Graphics::VulkanShaderPipeline::VulkanShaderPipeline(Device* device, const VertexLayout& layout, const PipelineSpecification& spec)
    : m_Device{dynamic_cast<VulkanDevice*>(device)}, m_VertexLayout{layout}, m_Specification{spec}
{
    vk::PushConstantRange pushConstantRange;
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pushConstantRange.size = sizeof(vk::DeviceAddress);
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    m_PipelineLayout = m_Device->GetVKLogicalDevice().createPipelineLayout(pipelineLayoutCreateInfo);

    vk::VertexInputBindingDescription vertexBinding;
    vertexBinding.binding = 0;
    vertexBinding.stride = m_VertexLayout.GetStride();
    vertexBinding.inputRate = vk::VertexInputRate::eVertex;

    std::vector<vk::VertexInputAttributeDescription> vertexAttributes;
    for (uint32_t index = 0; index < m_VertexLayout.GetElements().size(); index++)
    {
        vk::VertexInputAttributeDescription attribute;
        attribute.location = index;
        attribute.binding = 0;
        attribute.format = GetVulkanVertexFormat(m_VertexLayout.GetElements()[index].Type);
        attribute.offset = m_VertexLayout.GetElements()[index].Offset;
        vertexAttributes.push_back(attribute);
    }

    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    vertexInputStateCreateInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexBinding;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexAttributes.size();
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributes.data();

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
    inputAssemblyStateCreateInfo.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
    inputAssemblyStateCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    for (const auto& stage : m_Specification.stages)
    {
        Ref<VulkanShaderModule> vulkanShaderModule = std::dynamic_pointer_cast<VulkanShaderModule>(stage.Module);
        vk::PipelineShaderStageCreateInfo shaderStageCreateInfo;
        shaderStageCreateInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
        shaderStageCreateInfo.stage = GetVulkanShaderStage(stage.ShaderStage);
        shaderStageCreateInfo.module = vulkanShaderModule->GetVKShaderModule();
        shaderStageCreateInfo.pName = stage.EntryPoint.c_str();
        shaderStages.push_back(shaderStageCreateInfo);
    }

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo;
    viewportStateCreateInfo.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.scissorCount = 1;
    std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo;
    dynamicStateCreateInfo.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
    dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size();
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

    std::vector<vk::Format> colorAttachmentFormats;
    vk::Format depthAttachmentFormat;
    for (const auto& attachmentSpec : m_Specification.framebufferSpec.AttachmentSpec.Attachments)
    {
        if (Framebuffer::IsDepthFormat(attachmentSpec.InternalFormat) || Framebuffer::IsDepthStencilFormat(attachmentSpec.InternalFormat))
        {
            depthAttachmentFormat = VulkanTexture2D::GetVulkanInternalFormat(attachmentSpec.InternalFormat);
        }
        else
        {
            colorAttachmentFormats.push_back(VulkanTexture2D::GetVulkanInternalFormat(attachmentSpec.InternalFormat));
        }
    }

    vk::PipelineRenderingCreateInfo renderingCreateInfo;
    renderingCreateInfo.sType = vk::StructureType::ePipelineRenderingCreateInfo;
    renderingCreateInfo.colorAttachmentCount = colorAttachmentFormats.size();
    renderingCreateInfo.pColorAttachmentFormats = colorAttachmentFormats.data();
    renderingCreateInfo.depthAttachmentFormat = depthAttachmentFormat;

    vk::PipelineColorBlendAttachmentState blendAttachment;
    blendAttachment.blendEnable = m_Specification.blendState.enabled;
    blendAttachment.srcColorBlendFactor = GetVulkanBlendFactor(m_Specification.blendState.srcColorFactor);
    blendAttachment.dstColorBlendFactor = GetVulkanBlendFactor(m_Specification.blendState.dstColorFactor);
    blendAttachment.colorBlendOp = GetVulkanBlendOperation(m_Specification.blendState.colorOperation);
    blendAttachment.srcAlphaBlendFactor = GetVulkanBlendFactor(m_Specification.blendState.srcAlphaFactor);
    blendAttachment.dstAlphaBlendFactor = GetVulkanBlendFactor(m_Specification.blendState.dstAlphaFactor);
    blendAttachment.alphaBlendOp = GetVulkanBlendOperation(m_Specification.blendState.alphaOperation);
    blendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
    colorBlendStateCreateInfo.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &blendAttachment;

    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    rasterizationStateCreateInfo.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
    rasterizationStateCreateInfo.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    multisampleStateCreateInfo.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
    multisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
    pipelineCreateInfo.pNext = &renderingCreateInfo;
    pipelineCreateInfo.stageCount = shaderStages.size();
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.layout = m_PipelineLayout;

    m_Pipeline = m_Device->GetVKLogicalDevice().createGraphicsPipeline(nullptr, pipelineCreateInfo).value;
}

Nebula::Graphics::VulkanShaderPipeline::~VulkanShaderPipeline()
{
    m_Device->GetVKLogicalDevice().destroyPipeline(m_Pipeline);
    m_Device->GetVKLogicalDevice().destroyPipelineLayout(m_PipelineLayout);
}

uint32_t Nebula::Graphics::VulkanShaderPipeline::GetUniformBlockLocation(const std::string& uniformBlockName)
{
    uint32_t location = 0;
    for (const auto& stage : m_Specification.stages)
    {
        Ref<VulkanShaderModule> vulkanShaderModule = std::dynamic_pointer_cast<VulkanShaderModule>(stage.Module);
        ShaderReflectionLayout layout = vulkanShaderModule->GetLayout();
        if (layout.ResourceIndex.find(uniformBlockName) != layout.ResourceIndex.end())
        {
            ResourceBinding resource = layout.Resources[layout.ResourceIndex[uniformBlockName]];
            location = resource.Binding;
            break;
        }
    }
    return location;
}

vk::Format Nebula::Graphics::VulkanShaderPipeline::GetVulkanVertexFormat(ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::None: return vk::Format::eUndefined;
    case ShaderDataType::Float: return vk::Format::eR32Sfloat;
    case ShaderDataType::Float2: return vk::Format::eR32G32Sfloat;
    case ShaderDataType::Float3: return vk::Format::eR32G32B32Sfloat;
    case ShaderDataType::Float4: return vk::Format::eR32G32B32A32Sfloat;
    case ShaderDataType::Matrix3: return vk::Format::eR32G32B32Sfloat;
    case ShaderDataType::Matrix4: return vk::Format::eR32G32B32A32Sfloat;
    case ShaderDataType::Byte: return vk::Format::eR8Sint;
    case ShaderDataType::Byte2: return vk::Format::eR8G8Sint;
    case ShaderDataType::Byte3: return vk::Format::eR8G8B8Sint;
    case ShaderDataType::Byte4: return vk::Format::eR8G8B8A8Sint;
    case ShaderDataType::UByte: return vk::Format::eR8Uint;
    case ShaderDataType::UByte2: return vk::Format::eR8G8Uint;
    case ShaderDataType::UByte3: return vk::Format::eR8G8B8Uint;
    case ShaderDataType::UByte4: return vk::Format::eR8G8B8A8Uint;
    case ShaderDataType::Int: return vk::Format::eR32Sint;
    case ShaderDataType::Int2: return vk::Format::eR32G32Sint;
    case ShaderDataType::Int3: return vk::Format::eR32G32B32Sint;
    case ShaderDataType::Int4: return vk::Format::eR32G32B32A32Sint;
    case ShaderDataType::Bool: return vk::Format::eR8Unorm;
    }
}

vk::ShaderStageFlagBits Nebula::Graphics::VulkanShaderPipeline::GetVulkanShaderStage(ShaderStage stage)
{
    switch (stage)
    {
    case ShaderStage::Vertex: return vk::ShaderStageFlagBits::eVertex;
    case ShaderStage::Fragment: return vk::ShaderStageFlagBits::eFragment;
    case ShaderStage::Compute: return vk::ShaderStageFlagBits::eCompute;
    }
}

vk::BlendFactor Nebula::Graphics::VulkanShaderPipeline::GetVulkanBlendFactor(BlendFactor factor)
{
    switch (factor)
    {
    case BlendFactor::Zero: return vk::BlendFactor::eZero;
    case BlendFactor::One: return vk::BlendFactor::eOne;
    case BlendFactor::SourceAlpha: return vk::BlendFactor::eSrcAlpha;
    case BlendFactor::OneMinusSourceAlpha: return vk::BlendFactor::eOneMinusSrcAlpha;
    case BlendFactor::DestinationAlpha: return vk::BlendFactor::eDstAlpha;
    case BlendFactor::OneMinusDestinationAlpha: return vk::BlendFactor::eOneMinusDstAlpha;
    }
}

vk::BlendOp Nebula::Graphics::VulkanShaderPipeline::GetVulkanBlendOperation(BlendOperation operation)
{
    switch (operation)
    {
    case BlendOperation::Add: return vk::BlendOp::eAdd;
    case BlendOperation::Subtract: return vk::BlendOp::eSubtract;
    case BlendOperation::ReverseSubtract: return vk::BlendOp::eReverseSubtract;
    case BlendOperation::Minimum: return vk::BlendOp::eMin;
    case BlendOperation::Maximum: return vk::BlendOp::eMax;
    }
}