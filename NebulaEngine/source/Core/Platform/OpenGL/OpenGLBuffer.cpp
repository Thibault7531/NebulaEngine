#include "OpenGLBuffer.h"

#include <glad/gl.h>

namespace Nebula::Graphics {
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		NB_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, uint32_t size)
	{
		NB_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		NB_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		NB_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t size)
		: m_Count(size), m_DataType{BufferDataType::UInt32}
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint16_t* indices, uint32_t count)
		: m_Count(count), m_DataType{BufferDataType::UInt16}
	{
		NB_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint16_t), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count), m_DataType{BufferDataType::UInt32}
	{
		NB_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		NB_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::SetData(uint16_t* indices, uint32_t count, uint32_t offset)
	{
		m_Count = count;
		m_DataType = BufferDataType::UInt16;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, count * sizeof(uint16_t), indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLIndexBuffer::SetData(uint32_t* indices, uint32_t count, uint32_t offset)
	{
		m_Count = count;
		m_DataType = BufferDataType::UInt32;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, count * sizeof(uint32_t), indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	OpenGL460UniformBuffer::OpenGL460UniformBuffer(Device* device, const BufferLayout& layout, uint32_t instances)
		: m_Device{device}, m_Layout{layout}, m_InstanceCount{instances}
	{
		glCreateBuffers(1, &m_RendererID);

		uint32_t rawStride = layout.GetStride();
		uint32_t alignment = m_Device->GetUniformBufferAlignment();
		m_AlignedStride = (rawStride + alignment - 1) & ~(alignment - 1);

		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, m_AlignedStride * instances, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	OpenGL460UniformBuffer::~OpenGL460UniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGL460UniformBuffer::SetData(uint32_t index, const void* data)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferSubData(GL_UNIFORM_BUFFER, m_Layout.GetElements()[index].Offset, m_Layout.GetElements()[index].Size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGL460UniformBuffer::SetData(uint32_t instanceIndex, const std::string& paramater, const void* data)
	{
		for (auto& bufferParam : m_Layout.GetElements())
		{
			if (bufferParam.Name == paramater)
			{
				glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
				glBufferSubData(GL_UNIFORM_BUFFER, m_AlignedStride * instanceIndex + bufferParam.Offset, bufferParam.Size, data);
				glBindBuffer(GL_UNIFORM_BUFFER, 0);
				return;
			}
		}
	}

	void OpenGL460UniformBuffer::SetLayout(const BufferLayout& layout)
	{
		uint32_t rawStride = layout.GetStride();
		uint32_t alignment = m_Device->GetUniformBufferAlignment();
		m_AlignedStride = (rawStride + alignment - 1) & ~(alignment - 1);

		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, m_AlignedStride * m_InstanceCount, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		m_Layout = layout;
	}

}
