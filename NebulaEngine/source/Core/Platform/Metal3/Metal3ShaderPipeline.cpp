#include "Metal3ShaderPipeline.h"

#include "Metal3Texture.h"

namespace Nebula::Graphics
{

    Metal3ShaderPipeline::Metal3ShaderPipeline(MTL::Device* device, Ref<Metal3Shader> shader, VertexLayout layout,
        PipelineSpecification spec)
            : m_Shader{shader}, m_Spec{spec}
    {
        MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        renderPipelineDescriptor->setLabel(NS::String::string(shader->GetName().c_str(), NS::ASCIIStringEncoding));
        renderPipelineDescriptor->setVertexFunction(shader->GetVertexFunction());
        renderPipelineDescriptor->setFragmentFunction(shader->GetFragmentFunction());

        MTL::VertexDescriptor* vertexDescriptor = MTL::VertexDescriptor::vertexDescriptor();
        for (uint32_t index = 0; index < layout.GetElements().size(); index++)
        {
            VertexAttribute attribute = layout.GetElements()[index];
            vertexDescriptor->attributes()->object(index)->setFormat(GetMetal3VertexFormat(attribute.Type));
            vertexDescriptor->attributes()->object(index)->setOffset(attribute.Offset);
            vertexDescriptor->attributes()->object(index)->setBufferIndex(0);
        }
        vertexDescriptor->layouts()->object(0)->setStride(layout.GetStride());
        vertexDescriptor->layouts()->object(0)->setStepFunction(MTL::VertexStepFunctionPerVertex);
        vertexDescriptor->layouts()->object(0)->setStepRate(1);
        renderPipelineDescriptor->setVertexDescriptor(vertexDescriptor);

        uint32_t colorAttachmentIndex = 0;
        for (Texture::TextureSpecification attachmentSpec : spec.framebufferSpec.AttachmentSpec.Attachments)
        {
            if (Framebuffer::IsDepthStencilFormat(attachmentSpec.InternalFormat))
            {
                renderPipelineDescriptor->setDepthAttachmentPixelFormat(Metal3Texture2D::GetMetal3PixelFormat(attachmentSpec.InternalFormat));
            }
            else if (Framebuffer::IsDepthFormat(attachmentSpec.InternalFormat))
            {
                renderPipelineDescriptor->setDepthAttachmentPixelFormat(Metal3Texture2D::GetMetal3PixelFormat(attachmentSpec.InternalFormat));
            }
            else if (Framebuffer::IsStencilFormat(attachmentSpec.InternalFormat))
            {
                renderPipelineDescriptor->setStencilAttachmentPixelFormat(Metal3Texture2D::GetMetal3PixelFormat(attachmentSpec.InternalFormat));
            }
            else
            {
                MTL::RenderPipelineColorAttachmentDescriptor* colorAttachmentDescriptor = renderPipelineDescriptor->colorAttachments()->object(colorAttachmentIndex);
                colorAttachmentDescriptor->setPixelFormat(Metal3Texture2D::GetMetal3PixelFormat(attachmentSpec.InternalFormat));
                colorAttachmentDescriptor->setBlendingEnabled(spec.blendState.enabled);
                colorAttachmentDescriptor->setAlphaBlendOperation(GetMetal3BlendOperation(spec.blendState.alphaOperation));
                colorAttachmentDescriptor->setRgbBlendOperation(GetMetal3BlendOperation(spec.blendState.colorOperation));
                colorAttachmentDescriptor->setDestinationAlphaBlendFactor(GetMetal3BlendFactor(spec.blendState.dstAlphaFactor));
                colorAttachmentDescriptor->setDestinationRGBBlendFactor(GetMetal3BlendFactor(spec.blendState.dstColorFactor));
                colorAttachmentDescriptor->setSourceAlphaBlendFactor(GetMetal3BlendFactor(spec.blendState.srcAlphaFactor));
                colorAttachmentDescriptor->setSourceRGBBlendFactor(GetMetal3BlendFactor(spec.blendState.srcColorFactor));

                colorAttachmentIndex++;
            }
        }

        NS::Error* error;
        m_RenderPipelineState = device->newRenderPipelineState(renderPipelineDescriptor, &error);
        NB_CORE_ASSERT(m_RenderPipelineState, std::format("Could not compile shader {0}: {1}", shader->GetName(), error->localizedDescription()->utf8String()))

        renderPipelineDescriptor->release();
    }

    Metal3ShaderPipeline::~Metal3ShaderPipeline()
    {
        m_RenderPipelineState->release();
    }

    MTL::VertexFormat Metal3ShaderPipeline::GetMetal3VertexFormat(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::None: return MTL::VertexFormatInvalid;
        case ShaderDataType::Float: return MTL::VertexFormatFloat;
        case ShaderDataType::Float2: return MTL::VertexFormatFloat2;
        case ShaderDataType::Float3: return MTL::VertexFormatFloat3;
        case ShaderDataType::Float4: return MTL::VertexFormatFloat4;
        case ShaderDataType::Byte: return MTL::VertexFormatCharNormalized;
        case ShaderDataType::Byte2: return MTL::VertexFormatChar2Normalized;
        case ShaderDataType::Byte3: return MTL::VertexFormatChar3Normalized;
        case ShaderDataType::Byte4: return MTL::VertexFormatChar4Normalized;
        case ShaderDataType::UByte: return MTL::VertexFormatUCharNormalized;
        case ShaderDataType::UByte2: return MTL::VertexFormatUChar2Normalized;
        case ShaderDataType::UByte3: return MTL::VertexFormatUChar3Normalized;
        case ShaderDataType::UByte4: return MTL::VertexFormatUChar4Normalized;
        case ShaderDataType::Int: return MTL::VertexFormatInt;
        case ShaderDataType::Int2: return MTL::VertexFormatInt2;
        case ShaderDataType::Int3: return MTL::VertexFormatInt3;
        case ShaderDataType::Int4: return MTL::VertexFormatInt4;
        case ShaderDataType::Bool: return MTL::VertexFormatChar;
        }
    }

    MTL::BlendOperation Metal3ShaderPipeline::GetMetal3BlendOperation(BlendOperation operation)
    {
        switch (operation)
        {
        case BlendOperation::Add: return MTL::BlendOperationAdd;
        case BlendOperation::Subtract: return MTL::BlendOperationSubtract;
        case BlendOperation::ReverseSubtract: return MTL::BlendOperationReverseSubtract;
        case BlendOperation::Minimum: return MTL::BlendOperationMin;
        case BlendOperation::Maximum: return MTL::BlendOperationMax;
        }
    }

    MTL::BlendFactor Metal3ShaderPipeline::GetMetal3BlendFactor(BlendFactor factor)
    {
        switch (factor)
        {
        case BlendFactor::Zero: return MTL::BlendFactorZero;
        case BlendFactor::One: return MTL::BlendFactorOne;
        case BlendFactor::SourceAlpha: return MTL::BlendFactorSourceAlpha;
        case BlendFactor::OneMinusSourceAlpha: return MTL::BlendFactorOneMinusSourceAlpha;
        case BlendFactor::DestinationAlpha: return MTL::BlendFactorDestinationAlpha;
        case BlendFactor::OneMinusDestinationAlpha: return MTL::BlendFactorOneMinusDestinationAlpha;
        }
    }
}
