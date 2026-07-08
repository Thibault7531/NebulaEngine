#pragma once

#include <cstdint>

#include "Nebula/Renderer/Buffers/Buffer.h"

namespace Nebula {
	/**
	 * OpenGL Vertex buffer.
	 */
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(const float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void *data, uint32_t size) override;

		[[nodiscard]] virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	/**
	 * OpenGL Index buffer.
	 */
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer();
		OpenGLIndexBuffer(const uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(uint16_t* indices, uint32_t count) override;
		virtual void SetData(uint32_t* indices, uint32_t count) override;

		[[nodiscard]] virtual uint32_t GetCount() const override { return m_Count; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};

	class OpenGL460UniformBuffer : public UniformBuffer
	{
	public:
		OpenGL460UniformBuffer(const BufferLayout& layout);
		virtual ~OpenGL460UniformBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void BindToSlot(uint32_t slot) const override;

		virtual void SetData(uint32_t index, const void* data) override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override;

	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

}