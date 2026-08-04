#pragma once
#include <cstdint>
#include <vector>

#include "Core/Nebula/CoreSystems/Core/Core.h"
#include "Core/Nebula/CoreSystems/Core/Log.h"
#include "Nebula/Graphics/ShaderPipeline.h"

namespace Nebula::Graphics
{

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		BufferElement() = default;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		[[nodiscard]] uint32_t GetComponentCount() const
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

	class BufferLayout
	{
	public:
		BufferLayout() = default;

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		[[nodiscard]] inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		[[nodiscard]] inline uint32_t GetStride() const { return m_Stride; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		[[nodiscard]] std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		[[nodiscard]] std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

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
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride;
	};

	enum class BufferDataType
	{
		UInt16,
		UInt32
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		[[nodiscard]] virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		//static Ref<VertexBuffer> Create(uint32_t size);
		//static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void SetData(uint16_t* indices, uint32_t count, uint32_t offset = 0) = 0;
		virtual void SetData(uint32_t* indices, uint32_t count, uint32_t offset = 0) = 0;

		[[nodiscard]] virtual uint32_t GetCount() const = 0;
		virtual BufferDataType GetDataType() const = 0;

		//static Ref<IndexBuffer> Create();
		//static Ref<IndexBuffer> Create(uint32_t* Indices, uint32_t count);
	};

	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(uint32_t index, const void* data) = 0;
		virtual void SetData(const std::string& paramater, const void* data) = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

	public:
		static Ref<UniformBuffer> Create(const BufferLayout& layout);
	};


}
