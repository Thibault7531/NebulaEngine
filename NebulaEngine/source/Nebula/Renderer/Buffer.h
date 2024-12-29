#pragma once

namespace Nebula {
	/**
	 * Class listing all possible data types that can be input into a shader.
	 */
	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	/**
	 * Gets the size of any Shader data type.
	 * @param type Shader data type
	 * @return Size of the data type
	 */
	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float: return 4;
			case ShaderDataType::Float2: return 4 * 2;
			case ShaderDataType::Float3: return 4 * 3;
			case ShaderDataType::Float4: return 4 * 4;
			case ShaderDataType::Mat3: return 4 * 3 * 3;
			case ShaderDataType::Mat4: return 4 * 4 * 4;
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

	/**
	 * Information about data to input inside a shader.
	 */
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

		/**
		 * Gets the amount of component inside a data type.
		 * @return Amount of component inside a data type
		 */
		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case ShaderDataType::Float: return 1;
				case ShaderDataType::Float2: return 2;
				case ShaderDataType::Float3: return 3;
				case ShaderDataType::Float4: return 4;
				case ShaderDataType::Mat3: return 3 * 3;
				case ShaderDataType::Mat4: return 4 * 4;
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

	/**
	 * Class laying out all the data being inputted inside a shader.
	 */
	class BufferLayout
	{
	public:
		BufferLayout() = default;

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		/**
		 * Gets all the elements being inputted inside a shader.
		 * @return Vector with all the elements
		 */
		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		/**
		 * Gets the stride of the data being inputted inside a shader.
		 * @return Stride of the data
		 */
		inline uint32_t GetStride() const { return m_Stride; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

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

	/**
	 * Class representing a Vertex buffer object.
	 */
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		/**
		 * Gets the layout of the vertex buffer.
		 * @return Layout of the vertex buffer
		 */
		virtual const BufferLayout& GetLayout() const = 0;
		/**
		 * Sets the layout of the vertex buffer.
		 * @param layout Layout of the vertex buffer
		 */
		virtual void SetLayout(const BufferLayout& layout) = 0;

		/**
		 * Creates a vertex buffer with the inputted vertices.
		 * @param vertices Table with all the vertices
		 * @param size size of the table
		 * @return Pointer to the generated vertex buffer
		 */
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	/**
	 * Class representing an index buffer object.
	 */
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		/**
		 * Gets the amount of indices inside the index buffer.
		 * @return Amount of indices
		 */
		virtual uint32_t GetCount() const = 0;

		/**
		 * Creates an index buffer object with the inputted indices.
		 * @param Indices Table with all the indices
		 * @param count Amount of indices
		 * @return Pointer to the generated index buffer
		 */
		static Ref<IndexBuffer> Create(uint32_t* Indices, uint32_t count);
	};

}