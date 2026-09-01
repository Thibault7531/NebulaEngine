#pragma once

#include "Buffers/Framebuffer.h"

namespace Nebula::Graphics
{
    enum class ShaderDataType
    {
        None = 0,
        Float,
        Float2,
        Float3,
        Float4,
        Matrix3,
        Matrix4,
        Byte,
        Byte2,
        Byte3,
        Byte4,
        UByte,
        UByte2,
        UByte3,
        UByte4,
        Int,
        Int2,
        Int3,
        Int4,
        Bool
    };

    static uint32_t ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float: return 4;
        case ShaderDataType::Float2: return 4 * 2;
        case ShaderDataType::Float3: return 4 * 3;
        case ShaderDataType::Float4: return 4 * 4;
        case ShaderDataType::Matrix3: return 4 * 3 * 3;
        case ShaderDataType::Matrix4: return 4 * 4 * 4;
        case ShaderDataType::Byte: return 1;
        case ShaderDataType::Byte2: return 2;
        case ShaderDataType::Byte3: return 3;
        case ShaderDataType::Byte4: return 4;
        case ShaderDataType::UByte: return 1;
        case ShaderDataType::UByte2: return 2;
        case ShaderDataType::UByte3: return 3;
        case ShaderDataType::UByte4: return 4;
        case ShaderDataType::Int: return 4;
        case ShaderDataType::Int2: return 4 * 2;
        case ShaderDataType::Int3: return 4 * 3;
        case ShaderDataType::Int4: return 4 * 4;
        case ShaderDataType::Bool: return 1;
        default: break;
        }

        NB_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    struct VertexAttribute
    {
        std::string Name;
        ShaderDataType Type;
        uint32_t Size;
        uint32_t Offset;
        bool Normalized;

        VertexAttribute() = default;

        VertexAttribute(ShaderDataType type, const std::string& name, bool normalized = false)
            : Name{name}, Type{type}, Size{ShaderDataTypeSize(type)}, Offset{0}, Normalized{normalized} {}

        uint32_t GetComponentCount() const
        {
            switch (Type)
            {
            case ShaderDataType::Float: return 1;
            case ShaderDataType::Float2: return 2;
            case ShaderDataType::Float3: return 3;
            case ShaderDataType::Float4: return 4;
            case ShaderDataType::Matrix3: return 3 * 3;
            case ShaderDataType::Matrix4: return 4 * 4;
            case ShaderDataType::Byte: return 1;
            case ShaderDataType::Byte2: return 2;
            case ShaderDataType::Byte3: return 3;
            case ShaderDataType::Byte4: return 4;
            case ShaderDataType::UByte: return 1;
            case ShaderDataType::UByte2: return 2;
            case ShaderDataType::UByte3: return 3;
            case ShaderDataType::UByte4: return 4;
            case ShaderDataType::Int: return 1;
            case ShaderDataType::Int2: return 2;
            case ShaderDataType::Int3: return 3;
            case ShaderDataType::Int4: return 4;
            case ShaderDataType::Bool: return 1;
            default: break;
            }

            NB_CORE_ASSERT(false, "Unknown ShaderDataType!");
            return 0;
        }
    };

    class VertexLayout
    {
    public:
        VertexLayout() = default;
        VertexLayout(const std::initializer_list<VertexAttribute>& elements)
            : m_Elements(elements)
        {
            CalculateOffsetsAndStride();
        }

        inline const std::vector<VertexAttribute>& GetElements() const { return m_Elements; }
        inline uint32_t GetStride() const { return m_Stride; }

        std::vector<VertexAttribute>::iterator begin() { return m_Elements.begin(); }
        std::vector<VertexAttribute>::iterator end() { return m_Elements.end(); }
        std::vector<VertexAttribute>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<VertexAttribute>::const_iterator end() const { return m_Elements.end(); }

    private:
        void CalculateOffsetsAndStride()
        {
            uint32_t offset = 0;
            m_Stride = 0;
            for (auto& element : m_Elements)
            {
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }

    private:
        std::vector<VertexAttribute> m_Elements;
        uint32_t m_Stride;
    };

    enum class BlendFactor
    {
        Zero,
        One,
        SourceAlpha,
        OneMinusSourceAlpha,
        DestinationAlpha,
        OneMinusDestinationAlpha
    };

    enum class BlendOperation
    {
        Add,
        Subtract,
        ReverseSubtract,
        Minimum,
        Maximum
    };

    struct BlendState
    {
        bool enabled;

        BlendFactor srcColorFactor = BlendFactor::One;
        BlendFactor dstColorFactor = BlendFactor::Zero;
        BlendOperation colorOperation = BlendOperation::Add;

        BlendFactor srcAlphaFactor = BlendFactor::One;
        BlendFactor dstAlphaFactor = BlendFactor::Zero;
        BlendOperation alphaOperation = BlendOperation::Add;
    };

    struct PipelineSpecification
    {
        FramebufferSpecification framebufferSpec;
        BlendState blendState;
    };

    class ShaderPipeline
    {
    public:
        virtual ~ShaderPipeline() = default;

        virtual uint32_t GetUniformBlockLocation(const std::string& uniformBlockName) = 0;
        virtual uint32_t GetVertexInputLocation() = 0;
    };
}
