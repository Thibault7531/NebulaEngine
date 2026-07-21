#pragma once

#include "Core/Nebula/Graphics/Buffers/Buffer.h"
#include "Metal/Metal.hpp"

namespace Nebula::Graphics
{
    class Metal3VertexBuffer : public VertexBuffer
    {
    public:
        Metal3VertexBuffer(MTL::Device* device, uint32_t size);
        Metal3VertexBuffer(MTL::Device* device, const void* data, uint32_t size);
        ~Metal3VertexBuffer() override;

        virtual void SetData(const void* data, uint32_t size, uint32_t offset) override;

        virtual const BufferLayout& GetLayout() const override { return m_Layout; }
        virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

    public:
        MTL::Buffer* GetBuffer() { return m_Buffer; }

    private:
        BufferLayout m_Layout;

        MTL::Buffer* m_Buffer;
    };

    class Metal3IndexBuffer : public IndexBuffer
    {
    public:
        Metal3IndexBuffer(MTL::Device* device, uint32_t size);
        Metal3IndexBuffer(MTL::Device* device, uint16_t* indices, uint32_t count);
        Metal3IndexBuffer(MTL::Device* device, uint32_t* indices, uint32_t count);
        ~Metal3IndexBuffer() override;

        virtual void SetData(uint16_t* indices, uint32_t count, uint32_t offset) override;
        virtual void SetData(uint32_t* indices, uint32_t count, uint32_t offset) override;

        virtual uint32_t GetCount() const override { return m_Count; }
        virtual BufferDataType GetDataType() const override { return m_DataType; }

    public:
        MTL::Buffer* GetBuffer() { return m_Buffer; }

    public:
        static MTL::IndexType GetIndexType(BufferDataType type);

    private:
        MTL::Buffer* m_Buffer;

        uint32_t m_Count;
        BufferDataType m_DataType;
    };

    class Metal3UniformBuffer : public UniformBuffer
    {
    public:
        Metal3UniformBuffer(MTL::Device* device, const BufferLayout& layout);
        ~Metal3UniformBuffer() override;

        virtual void SetData(uint32_t index, const void* data) override;

        virtual const BufferLayout& GetLayout() const override { return m_Layout; }
        virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

    public:
        MTL::Buffer* GetBuffer() { return m_Buffer; }

    private:
        BufferLayout m_Layout;

        MTL::Buffer* m_Buffer;
    };

}
