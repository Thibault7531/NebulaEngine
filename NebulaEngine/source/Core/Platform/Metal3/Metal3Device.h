#pragma once

#include "Core/Nebula/Graphics/Device.h"
#include "Metal/Metal.hpp"
#include "Nebula/Graphics/Texture.h"

namespace Nebula::Graphics
{

    class Metal3Device : public Device
    {
    public:
        Metal3Device();
        virtual ~Metal3Device();

        virtual void SetSurfaceDevice(Ref<Surface> surface) override;

        virtual void WaitIdle() override;

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

        virtual Ref<SwapChain> CreateSwapchain(Ref<Surface> surface, VSyncMode vsyncMode, const FramebufferSpecification& framebufferSpec, uint32_t swapchainTextureIndex) override;

        virtual Ref<ShaderModule> CreateShaderModule(const std::filesystem::path& path) override;
        virtual Ref<ShaderPipeline> CreateShaderPipeline(const VertexLayout& layout, const PipelineSpecification& spec) override;

    public:
        virtual uint32_t GetMaximumTextureSize() override;
        virtual uint32_t GetUniformBufferAlignment() override;

    public:
        MTL::Device* GetMetalDevice() { return m_Device; }
        Ref<Queue> GetGraphicsQueue() { return m_GraphicsQueue; }

    private:
        MTL::Device* m_Device;

        Ref<Queue> m_GraphicsQueue;
    };

}
