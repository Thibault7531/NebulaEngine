#include "OpenGL410Buffer.h"

#include <glad/gl.h>

#include "Core/Nebula/Debug/Instrumentor.h"

namespace Nebula {
	OpenGL410VertexBuffer::OpenGL410VertexBuffer(uint32_t size)
	{
		NB_PROFILE_FUNCTION();

        glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	OpenGL410VertexBuffer::OpenGL410VertexBuffer(const float* vertices, uint32_t size)
	{
		NB_PROFILE_FUNCTION();

        glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	OpenGL410VertexBuffer::~OpenGL410VertexBuffer()
	{
		NB_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGL410VertexBuffer::Bind() const
	{
		NB_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGL410VertexBuffer::Unbind() const
	{
		NB_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGL410VertexBuffer::SetData(const void *data, uint32_t size)
	{
		NB_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	OpenGL410IndexBuffer::OpenGL410IndexBuffer(const uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		NB_PROFILE_FUNCTION();

        glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	OpenGL410IndexBuffer::~OpenGL410IndexBuffer()
	{
		NB_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGL410IndexBuffer::Bind() const
	{
		NB_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGL410IndexBuffer::Unbind() const
	{
		NB_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}
