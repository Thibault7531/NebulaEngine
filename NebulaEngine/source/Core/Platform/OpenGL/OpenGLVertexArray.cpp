#include "OpenGLVertexArray.h"

#include "OpenGLBuffer.h"

namespace Nebula::Graphics {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float: return GL_FLOAT;
		case ShaderDataType::Float2: return GL_FLOAT;
		case ShaderDataType::Float3: return GL_FLOAT;
		case ShaderDataType::Float4: return GL_FLOAT;
		case ShaderDataType::Matrix3: return GL_FLOAT;
		case ShaderDataType::Matrix4: return GL_FLOAT;
		case ShaderDataType::Byte: return GL_BYTE;
		case ShaderDataType::Byte2: return GL_BYTE;
		case ShaderDataType::Byte3: return GL_BYTE;
		case ShaderDataType::Byte4: return GL_BYTE;
		case ShaderDataType::UByte: return GL_UNSIGNED_BYTE;
		case ShaderDataType::UByte2: return GL_UNSIGNED_BYTE;
		case ShaderDataType::UByte3: return GL_UNSIGNED_BYTE;
		case ShaderDataType::UByte4: return GL_UNSIGNED_BYTE;
		case ShaderDataType::Int: return GL_INT;
		case ShaderDataType::Int2: return GL_INT;
		case ShaderDataType::Int3: return GL_INT;
		case ShaderDataType::Int4: return GL_INT;
		case ShaderDataType::Bool: return GL_BOOL;
		default: break;
		}

		return GL_INVALID_ENUM;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		NB_PROFILE_FUNCTION();

		glGenVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		NB_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		NB_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		Ref<OpenGLVertexBuffer> vbo = std::static_pointer_cast<OpenGLVertexBuffer>(vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vbo->GetBufferID());

		NB_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer Layout is not correct");

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
			case ShaderDataType::None: break;

			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
				{
					glEnableVertexAttribArray(index);
					glVertexAttribPointer(index, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), reinterpret_cast<const void*>(element.Offset));
					index++;
					break;
				}

			case ShaderDataType::Matrix3:
			case ShaderDataType::Matrix4:
				{
					uint8_t count = element.GetComponentCount();
					for (uint8_t i = 0; i < count; i++)
					{
						glEnableVertexAttribArray(index);
						glVertexAttribPointer(index, count, ShaderDataTypeToOpenGLBaseType(element.Type), element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), reinterpret_cast<const void*>(element.Offset + sizeof(float) * i));
						glVertexAttribDivisor(index, 1);
						index++;
					}
					break;
				}

			case ShaderDataType::Byte:
			case ShaderDataType::Byte2:
			case ShaderDataType::Byte3:
			case ShaderDataType::Byte4:
			case ShaderDataType::UByte:
			case ShaderDataType::UByte2:
			case ShaderDataType::UByte3:
			case ShaderDataType::UByte4:
				{
					glEnableVertexAttribArray(index);
					glVertexAttribPointer(index, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), GL_TRUE, layout.GetStride(), reinterpret_cast<const void*>(element.Offset));
					index++;
					break;
				}

			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Bool:
				{
					glEnableVertexAttribArray(index);
					glVertexAttribIPointer(index, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), layout.GetStride(), reinterpret_cast<const void*>(element.Offset));
					index++;
					break;
				}
			}
		}

		m_VertexBuffers.push_back(vertexBuffer);

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		NB_PROFILE_FUNCTION();

		Ref<OpenGLIndexBuffer> ibo = std::static_pointer_cast<OpenGLIndexBuffer>(indexBuffer);

		glBindVertexArray(m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->GetBufferID());
		m_IndexBuffer = indexBuffer;
		glBindVertexArray(0);
	}

}
