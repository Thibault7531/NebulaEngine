#pragma once

#include <glad/gl.h>

#include "Nebula/Graphics/Buffers/Buffer.h"

namespace Nebula::Graphics {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(const void* data, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void SetData(const void* data, uint32_t size, uint32_t offset) override;

		[[nodiscard]] virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

	public:
		GLuint GetBufferID() const { return m_RendererID; }

	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t size);
		OpenGLIndexBuffer(uint16_t* indices, uint32_t count);
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void SetData(uint16_t* indices, uint32_t count, uint32_t offset) override;
		virtual void SetData(uint32_t* indices, uint32_t count, uint32_t offset) override;

		[[nodiscard]] virtual uint32_t GetCount() const override { return m_Count; }
		virtual BufferDataType GetDataType() const override { return m_DataType; }

	public:
		GLuint GetBufferID() const { return m_RendererID; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
		BufferDataType m_DataType;
	};

	class OpenGL460UniformBuffer : public UniformBuffer
	{
	public:
		OpenGL460UniformBuffer(const BufferLayout& layout, uint32_t instances);
		virtual ~OpenGL460UniformBuffer();

		virtual void SetData(uint32_t index, const void* data) override;
		virtual void SetData(uint32_t instanceIndex, const std::string& paramater, const void* data) override;

		virtual uint32_t GetInstanceCount() const override { return m_InstanceCount; }

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override;

	public:
		GLuint GetBufferID() const { return m_RendererID; }

	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
		uint32_t m_InstanceCount;
	};

}