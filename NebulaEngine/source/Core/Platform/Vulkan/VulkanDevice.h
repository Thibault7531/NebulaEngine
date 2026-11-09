#pragma once

#include <vulkan/vulkan.hpp>

#include "vk_mem_alloc.hpp"
#include "Nebula/Graphics/Device.h"

namespace Nebula::Graphics
{
    class VulkanDevice : public Device
    {
    public:
        VulkanDevice();
        ~VulkanDevice();

        virtual void SetSurfaceDevice(Ref<Surface> surface) override;

        virtual Ref<VertexBuffer> CreateVertexBuffer(uint32_t size) override;
        virtual Ref<VertexBuffer> CreateVertexBuffer(const void* data, uint32_t size) override;

        virtual Ref<IndexBuffer> CreateIndexBuffer(uint32_t count) override;
        virtual Ref<IndexBuffer> CreateIndexBuffer(uint16_t* data, uint32_t count) override;
        virtual Ref<IndexBuffer> CreateIndexBuffer(uint32_t* data, uint32_t count) override;

        virtual Ref<UniformBuffer> CreateUniformBuffer(const BufferLayout& layout, uint32_t instances) override;

        virtual Ref<Texture2D> CreateTexture2D(uint32_t width, uint32_t height, const Texture::TextureSpecification& spec) override;
        virtual Ref<Texture2D> CreateTexture2D(std::filesystem::path path, const Texture::TextureSpecification& spec) override;

        virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification& spec) override;
        virtual Ref<Framebuffer> CreateFramebufferForSwapchain(const FramebufferSpecification& spec, uint32_t swapchainTextureIndex, Ref<Texture2D> swapchainTexture) override;

        virtual Ref<SwapChain> CreateSwapchain(Ref<Surface> surface, const FramebufferSpecification& framebufferSpec, uint32_t swapchainTextureIndex) override;

        virtual Ref<ShaderModule> CreateShaderModule(const std::filesystem::path& path) override;
        virtual Ref<ShaderPipeline> CreateShaderPipeline(const VertexLayout& layout, const PipelineSpecification& spec) override;

    public:
        virtual uint32_t GetMaximumTextureSize() override;
        virtual uint32_t GetUniformBufferAlignment() override;

        virtual Ref<Queue> GetGraphicsQueue() override { return m_GraphicsQueue; }

    public:
        vk::PhysicalDevice GetVKPhysicalDevice() const { return m_PhysicalDevice; }
        vk::Device GetVKLogicalDevice() const { return m_LogicalDevice; }
        uint32_t GetVKGraphicsQueueFamily() const { return m_GraphicsQueueFamily; }
        vma::Allocator GetVMAAllocator() const { return m_Allocator; }

    private:
        vk::PhysicalDevice m_PhysicalDevice;
        vk::Device m_LogicalDevice;
        vma::Allocator m_Allocator;

        uint32_t m_GraphicsQueueFamily;
        Ref<Queue> m_GraphicsQueue;

        static bool s_InstanceInitialized;
        static vk::Instance s_Instance;
    };
}
