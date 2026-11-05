#pragma once

#include "VulkanDevice.h"
#include "Nebula/Graphics/Buffers/Buffer.h"

namespace Nebula::Graphics
{
    class VulkanVertexBuffer : public VertexBuffer
    {
    public:
        VulkanVertexBuffer(Device* device, uint32_t size);
        VulkanVertexBuffer(Device* device, const void* data, uint32_t size);
        ~VulkanVertexBuffer() override;

        virtual void SetData(const void* data, uint32_t size, uint32_t offset) override;

        virtual const BufferLayout& GetLayout() const override { return m_Layout; }
        virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

    private:
        VulkanDevice* m_Device;
        BufferLayout m_Layout;

        vk::Buffer m_Buffer;
        vma::Allocation m_Allocation;
        vma::AllocationInfo m_AllocationInfo;
    };

    class VulkanIndexBuffer : public IndexBuffer
    {
    public:
        VulkanIndexBuffer(Device* device, uint32_t count);
        VulkanIndexBuffer(Device* device, uint16_t* data, uint32_t count);
        VulkanIndexBuffer(Device* device, uint32_t* data, uint32_t count);
        ~VulkanIndexBuffer() override;

        virtual void SetData(uint16_t* indices, uint32_t count, uint32_t offset) override;
        virtual void SetData(uint32_t* indices, uint32_t count, uint32_t offset) override;

        virtual uint32_t GetCount() const override { return m_Count; }
        virtual BufferDataType GetDataType() const override { return m_DataType; }

    private:
        VulkanDevice* m_Device;
        uint32_t m_Count;
        BufferDataType m_DataType;

        vk::Buffer m_Buffer;
        vma::Allocation m_Allocation;
        vma::AllocationInfo m_AllocationInfo;
    };

    class VulkanUniformBuffer : public UniformBuffer
    {
    public:
        VulkanUniformBuffer(Device* device, const BufferLayout& layout, uint32_t instances = 1);
        ~VulkanUniformBuffer() override;

        virtual void SetData(uint32_t index, const void* data) override;
        virtual void SetData(uint32_t instanceIndex, const std::string& paramater, const void* data) override;

        virtual uint32_t GetInstanceCount() const override { return m_Instances; }

        virtual const BufferLayout& GetLayout() const override { return m_Layout; }
        virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

    private:
        VulkanDevice* m_Device;
        BufferLayout m_Layout;
        uint32_t m_Instances;

        vk::Buffer m_Buffer;
        vma::Allocation m_Allocation;
        vma::AllocationInfo m_AllocationInfo;
    };
}
