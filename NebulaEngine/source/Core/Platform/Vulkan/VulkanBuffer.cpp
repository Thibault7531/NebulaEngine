#include "VulkanBuffer.h"

namespace Nebula::Graphics
{
    VulkanVertexBuffer::VulkanVertexBuffer(Device* device, uint32_t size)
        : m_Device{dynamic_cast<VulkanDevice*>(device)}
    {
        vk::BufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;

        vma::AllocationCreateInfo allocationCreateInfo;
        allocationCreateInfo.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead | vma::AllocationCreateFlagBits::eMapped;
        allocationCreateInfo.usage = vma::MemoryUsage::eAuto;

        auto allocResult = m_Device->GetVMAAllocator().createBuffer(bufferCreateInfo, allocationCreateInfo, m_AllocationInfo);
        m_Allocation = allocResult.first;
        m_Buffer = allocResult.second;
    }

    VulkanVertexBuffer::VulkanVertexBuffer(Device* device, const void* data, uint32_t size)
    {
        m_Device = dynamic_cast<VulkanDevice*>(device);

        vk::BufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;

        vma::AllocationCreateInfo allocationCreateInfo;
        allocationCreateInfo.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead | vma::AllocationCreateFlagBits::eMapped;
        allocationCreateInfo.usage = vma::MemoryUsage::eAuto;

        auto allocResult = m_Device->GetVMAAllocator().createBuffer(bufferCreateInfo, allocationCreateInfo, m_AllocationInfo);
        m_Allocation = allocResult.first;
        m_Buffer = allocResult.second;

        memcpy(m_AllocationInfo.pMappedData, data, size);
    }

    VulkanVertexBuffer::~VulkanVertexBuffer()
    {
        m_Device->GetVMAAllocator().destroyBuffer(m_Buffer, m_Allocation);
    }

    void VulkanVertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
    {
        memcpy(static_cast<char*>(m_AllocationInfo.pMappedData) + offset, data, size);
        m_Device->GetVMAAllocator().flushAllocation(m_Allocation, offset, size);
    }

    VulkanIndexBuffer::VulkanIndexBuffer(Device* device, uint32_t count)
        : m_Device{dynamic_cast<VulkanDevice*>(device)}, m_Count{count}, m_DataType{BufferDataType::UInt32}
    {
        vk::BufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
        bufferCreateInfo.size = count * sizeof(uint32_t);
        bufferCreateInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer;

        vma::AllocationCreateInfo allocationCreateInfo;
        allocationCreateInfo.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead | vma::AllocationCreateFlagBits::eMapped;
        allocationCreateInfo.usage = vma::MemoryUsage::eAuto;

        auto allocResult = m_Device->GetVMAAllocator().createBuffer(bufferCreateInfo, allocationCreateInfo, m_AllocationInfo);
        m_Allocation = allocResult.first;
        m_Buffer = allocResult.second;
    }

    VulkanIndexBuffer::VulkanIndexBuffer(Device* device, uint16_t* data, uint32_t count)
        : m_Device{dynamic_cast<VulkanDevice*>(device)}, m_Count{count}, m_DataType{BufferDataType::UInt16}
    {
        vk::BufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
        bufferCreateInfo.size = count * sizeof(uint16_t);
        bufferCreateInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer;

        vma::AllocationCreateInfo allocationCreateInfo;
        allocationCreateInfo.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead | vma::AllocationCreateFlagBits::eMapped;
        allocationCreateInfo.usage = vma::MemoryUsage::eAuto;

        auto allocResult = m_Device->GetVMAAllocator().createBuffer(bufferCreateInfo, allocationCreateInfo, m_AllocationInfo);
        m_Allocation = allocResult.first;
        m_Buffer = allocResult.second;

        memcpy(m_AllocationInfo.pMappedData, data, count * sizeof(uint16_t));
    }

    VulkanIndexBuffer::VulkanIndexBuffer(Device* device, uint32_t* data, uint32_t count)
        : m_Device{dynamic_cast<VulkanDevice*>(device)}, m_Count{count}, m_DataType{BufferDataType::UInt32}
    {
        vk::BufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
        bufferCreateInfo.size = count * sizeof(uint32_t);
        bufferCreateInfo.usage = vk::BufferUsageFlagBits::eIndexBuffer;

        vma::AllocationCreateInfo allocationCreateInfo;
        allocationCreateInfo.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead | vma::AllocationCreateFlagBits::eMapped;
        allocationCreateInfo.usage = vma::MemoryUsage::eAuto;

        auto allocResult = m_Device->GetVMAAllocator().createBuffer(bufferCreateInfo, allocationCreateInfo, m_AllocationInfo);
        m_Allocation = allocResult.first;
        m_Buffer = allocResult.second;

        memcpy(m_AllocationInfo.pMappedData, data, count * sizeof(uint32_t));
    }

    VulkanIndexBuffer::~VulkanIndexBuffer()
    {
        m_Device->GetVMAAllocator().destroyBuffer(m_Buffer, m_Allocation);
    }

    void VulkanIndexBuffer::SetData(uint16_t* indices, uint32_t count, uint32_t offset)
    {
        m_DataType = BufferDataType::UInt16;
        m_Count = count;
        memcpy(static_cast<char*>(m_AllocationInfo.pMappedData) + offset, indices, count * sizeof(uint16_t));
        m_Device->GetVMAAllocator().flushAllocation(m_Allocation, offset, count * sizeof(uint16_t));
    }

    void VulkanIndexBuffer::SetData(uint32_t* indices, uint32_t count, uint32_t offset)
    {
        m_DataType = BufferDataType::UInt32;
        m_Count = count;
        memcpy(static_cast<char*>(m_AllocationInfo.pMappedData) + offset, indices, count * sizeof(uint32_t));
        m_Device->GetVMAAllocator().flushAllocation(m_Allocation, offset, count * sizeof(uint32_t));
    }

    VulkanUniformBuffer::VulkanUniformBuffer(Device* device, const BufferLayout& layout, uint32_t instances)
        : m_Device{dynamic_cast<VulkanDevice*>(device)}, m_Layout{layout}, m_Instances{instances}
    {
        vk::BufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
        bufferCreateInfo.size = layout.GetStride() * instances;
        bufferCreateInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;

        vma::AllocationCreateInfo allocationCreateInfo;
        allocationCreateInfo.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead | vma::AllocationCreateFlagBits::eMapped;
        allocationCreateInfo.usage = vma::MemoryUsage::eAuto;

        auto allocResult = m_Device->GetVMAAllocator().createBuffer(bufferCreateInfo, allocationCreateInfo, m_AllocationInfo);
        m_Allocation = allocResult.first;
        m_Buffer = allocResult.second;
    }

    VulkanUniformBuffer::~VulkanUniformBuffer()
    {
        m_Device->GetVMAAllocator().destroyBuffer(m_Buffer, m_Allocation);
    }

    void VulkanUniformBuffer::SetData(uint32_t index, const void* data)
    {
        memcpy(static_cast<char*>(m_AllocationInfo.pMappedData) + m_Layout.GetElements()[index].Offset, data, m_Layout.GetElements()[index].Size);
    }

    void VulkanUniformBuffer::SetData(uint32_t instanceIndex, const std::string& paramater, const void* data)
    {
        for (const auto& bufferParam : m_Layout.GetElements())
        {
            if (bufferParam.Name == paramater)
            {
                memcpy(static_cast<uint8_t*>(m_AllocationInfo.pMappedData) + instanceIndex * m_Layout.GetStride() + bufferParam.Offset, data, bufferParam.Size);
                return;
            }
        }
    }
}
