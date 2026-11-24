#include "Metal3Device.h"

#include "Metal3Buffer.h"
#include "Metal3CommandBuffer.h"
#include "Metal3Framebuffer.h"
#include "Metal3Queue.h"
#include "Metal3ShaderModule.h"
#include "Metal3ShaderPipeline.h"
#include "Metal3Swapchain.h"
#include "Metal3Texture.h"

namespace Nebula::Graphics
{

    Metal3Device::Metal3Device()
        : m_Device{MTL::CreateSystemDefaultDevice()}
    {
        m_GraphicsQueue = CreateRef<Metal3Queue>(this);
    }

    Metal3Device::~Metal3Device()
    {
        m_Device->release();
    }

    void Metal3Device::SetSurfaceDevice(Ref<Surface> surface)
    {
        Ref<Metal3Queue> queue = std::dynamic_pointer_cast<Metal3Queue>(m_GraphicsQueue);
        surface->SetDevice(nullptr, m_Device, queue->GetMetalCommandQueue());
    }

    void Metal3Device::WaitIdle()
    {
        Ref<Metal3Queue> queue = std::dynamic_pointer_cast<Metal3Queue>(m_GraphicsQueue);
        MTL::CommandQueue* commandQueue = queue->GetMetalCommandQueue();
        MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
        commandBuffer->commit();
        commandBuffer->waitUntilCompleted();
    }

    Ref<VertexBuffer> Metal3Device::CreateVertexBuffer(uint32_t size)
    {
        return CreateRef<Metal3VertexBuffer>(m_Device, size);
    }

    Ref<VertexBuffer> Metal3Device::CreateVertexBuffer(const void* data, uint32_t size)
    {
        return CreateRef<Metal3VertexBuffer>(m_Device, data, size);
    }

    Ref<IndexBuffer> Metal3Device::CreateIndexBuffer(uint32_t count)
    {
        return CreateRef<Metal3IndexBuffer>(m_Device, count);
    }

    Ref<IndexBuffer> Metal3Device::CreateIndexBuffer(uint16_t* data, uint32_t count)
    {
        return CreateRef<Metal3IndexBuffer>(m_Device, data, count);
    }

    Ref<IndexBuffer> Metal3Device::CreateIndexBuffer(uint32_t* data, uint32_t count)
    {
        return CreateRef<Metal3IndexBuffer>(m_Device, data, count);
    }

    Ref<UniformBuffer> Metal3Device::CreateUniformBuffer(const BufferLayout& layout, uint32_t instances)
    {
        return CreateRef<Metal3UniformBuffer>(m_Device, layout, instances);
    }

    Ref<Texture2D> Metal3Device::CreateTexture2D(uint32_t width, uint32_t height, const Texture::TextureSpecification& spec)
    {
        return CreateRef<Metal3Texture2D>(m_Device, width, height, spec);
    }

    Ref<Texture2D> Metal3Device::CreateTexture2D(std::filesystem::path path, const Texture::TextureSpecification& spec)
    {
        return CreateRef<Metal3Texture2D>(m_Device, path, spec);
    }

    Ref<Framebuffer> Metal3Device::CreateFramebuffer(const FramebufferSpecification& spec)
    {
        return CreateRef<Metal3Framebuffer>(this, spec);
    }

    Ref<Framebuffer> Metal3Device::CreateFramebufferForSwapchain(const FramebufferSpecification& spec, uint32_t swapchainTextureIndex, Ref<Texture2D> swapchainTexture)
    {
        return CreateRef<Metal3Framebuffer>(this, spec, swapchainTextureIndex, swapchainTexture);
    }

    Ref<SwapChain> Metal3Device::CreateSwapchain(Ref<Surface> surface, VSyncMode vsyncMode, const FramebufferSpecification& framebufferSpec, uint32_t swapchainTextureIndex)
    {
        return CreateRef<Metal3Swapchain>(this, surface, vsyncMode, framebufferSpec, swapchainTextureIndex);
    }

    Ref<ShaderModule> Metal3Device::CreateShaderModule(const std::filesystem::path& path)
    {
        return CreateRef<Metal3ShaderModule>(m_Device, path);
    }

    Ref<ShaderPipeline> Metal3Device::CreateShaderPipeline(const VertexLayout& layout, const PipelineSpecification& spec)
    {
        return CreateRef<Metal3ShaderPipeline>(m_Device, layout, spec);
    }

    uint32_t Metal3Device::GetMaximumTextureSize()
    {
        if (m_Device->supportsFamily(MTL::GPUFamilyMacCatalyst1) || m_Device->supportsFamily(MTL::GPUFamilyMac1) || m_Device->supportsFamily(MTL::GPUFamilyApple3)) return 16384;
        return 8192;
    }

    uint32_t Metal3Device::GetUniformBufferAlignment()
    {
        return 1;
    }
}
