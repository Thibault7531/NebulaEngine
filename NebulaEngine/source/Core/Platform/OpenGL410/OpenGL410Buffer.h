#pragma once

#include <cstdint>

#include "Core/Nebula/Renderer/Buffers/Buffer.h"

namespace Nebula {

	class OpenGL410VertexBuffer : public VertexBuffer
	{
	public:
		OpenGL410VertexBuffer(uint32_t size);
		OpenGL410VertexBuffer(const float* vertices, uint32_t size);
		virtual ~OpenGL410VertexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void *data, uint32_t size) override;

		[[nodiscard]] virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	class OpenGL410IndexBuffer : public IndexBuffer
	{
	public:
		OpenGL410IndexBuffer();
		OpenGL410IndexBuffer(const uint32_t* indices, uint32_t count);
		virtual ~OpenGL410IndexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(uint16_t* indices, uint32_t count) override;
		virtual void SetData(uint32_t* indices, uint32_t count) override;

		[[nodiscard]] virtual uint32_t GetCount() const override { return m_Count; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};

}