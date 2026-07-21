#include "Metal3Buffer.h"

namespace Nebula::Graphics
{

    Metal3VertexBuffer::Metal3VertexBuffer(MTL::Device* device, uint32_t size)
    {
        m_Buffer = device->newBuffer(size ? size : 4, MTL::ResourceStorageModeShared);
    }

    Metal3VertexBuffer::Metal3VertexBuffer(MTL::Device* device, const void* data, uint32_t size)
    {
        m_Buffer = device->newBuffer(data, size, MTL::ResourceStorageModeShared);
    }

    Metal3VertexBuffer::~Metal3VertexBuffer()
    {
        m_Buffer->release();
    }

    void Metal3VertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
    {
        memcpy(static_cast<uint8_t*>(m_Buffer->contents()) + offset, data, size);
    }

    Metal3IndexBuffer::Metal3IndexBuffer(MTL::Device* device, uint32_t size)
    {
        m_Buffer = device->newBuffer(size ? size : 3 * 4, MTL::ResourceStorageModeShared);
    }

    Metal3IndexBuffer::Metal3IndexBuffer(MTL::Device* device, uint16_t* indices, uint32_t count)
        : m_Count{count}, m_DataType{BufferDataType::UInt16}
    {
        m_Buffer = device->newBuffer(indices, count * 2, MTL::ResourceStorageModeShared);
    }

    Metal3IndexBuffer::Metal3IndexBuffer(MTL::Device* device, uint32_t* indices, uint32_t count)
        : m_Count{count}, m_DataType{BufferDataType::UInt32}
    {
        m_Buffer = device->newBuffer(indices, count * 4, MTL::ResourceStorageModeShared);
    }

    Metal3IndexBuffer::~Metal3IndexBuffer()
    {
        m_Buffer->release();
    }

    void Metal3IndexBuffer::SetData(uint16_t* indices, uint32_t count, uint32_t offset)
    {
        m_Count = count;
        m_DataType = BufferDataType::UInt16;
        memcpy(static_cast<uint8_t*>(m_Buffer->contents()) + offset, indices, count * 2);
    }

    void Metal3IndexBuffer::SetData(uint32_t* indices, uint32_t count, uint32_t offset)
    {
        m_Count = count;
        m_DataType = BufferDataType::UInt32;
        memcpy(static_cast<uint8_t*>(m_Buffer->contents()) + offset, indices, count * 4);
    }

    MTL::IndexType Metal3IndexBuffer::GetIndexType(BufferDataType type)
    {
        switch (type)
        {
        case BufferDataType::UInt16: return MTL::IndexTypeUInt16;
        case BufferDataType::UInt32: return MTL::IndexTypeUInt32;
        }
    }

    Metal3UniformBuffer::Metal3UniformBuffer(MTL::Device* device, const BufferLayout& layout)
        : m_Layout{layout}
    {
        m_Buffer = device->newBuffer(layout.GetStride(), MTL::ResourceStorageModeShared);
    }

    Metal3UniformBuffer::~Metal3UniformBuffer()
    {
        m_Buffer->release();
    }

    void Metal3UniformBuffer::SetData(uint32_t index, const void* data)
    {
        memcpy(static_cast<uint8_t*>(m_Buffer->contents()) + m_Layout.GetElements()[index].Offset, data, m_Layout.GetElements()[index].Size);
    }
}
