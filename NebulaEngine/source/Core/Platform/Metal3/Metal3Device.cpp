#include "Metal3Device.h"

#include "Metal3Buffer.h"
#include "Metal3CommandBuffer.h"
#include "Metal3Framebuffer.h"
#include "Metal3Shader.h"
#include "Metal3ShaderPipeline.h"
#include "Metal3Surface.h"
#include "Metal3Texture.h"

namespace Nebula::Graphics
{

    Metal3Device::Metal3Device()
        : m_Device{MTL::CreateSystemDefaultDevice()}, m_CommandQueue{m_Device->newCommandQueue()}
    {
    }

    Metal3Device::~Metal3Device()
    {
        m_CommandQueue->release();
        m_Device->release();
    }

    void Metal3Device::SetSurfaceDevice(Ref<Surface> surface)
    {
        Ref<Metal3Surface> nativeSurface = std::dynamic_pointer_cast<Metal3Surface>(surface);
        nativeSurface->SetDevice(m_Device, m_CommandQueue);
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

    Ref<UniformBuffer> Metal3Device::CreateUniformBuffer(const BufferLayout& layout)
    {
        return CreateRef<Metal3UniformBuffer>(m_Device, layout);
    }

    Ref<Texture2D> Metal3Device::CreateTexture2D(uint32_t width, uint32_t height,
        const Texture::TextureSpecification& spec)
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

    Ref<Shader> Metal3Device::CreateShader(const std::filesystem::path& path)
    {
        return CreateRef<Metal3Shader>(m_Device, path);
    }

    Ref<ShaderPipeline> Metal3Device::CreateShaderPipeline(Ref<Shader> shader, VertexLayout layout, const PipelineSpecification& spec)
    {
        return CreateRef<Metal3ShaderPipeline>(m_Device, std::dynamic_pointer_cast<Metal3Shader>(shader), layout, spec);
    }

    Ref<CommandBuffer> Metal3Device::CreateCommandBuffer()
    {
        return CreateRef<Metal3CommandBuffer>(m_Device, m_CommandQueue);
    }

    uint32_t Metal3Device::GetMaximumTextureSize()
    {
        if (m_Device->supportsFamily(MTL::GPUFamilyMacCatalyst1) || m_Device->supportsFamily(MTL::GPUFamilyMac1) || m_Device->supportsFamily(MTL::GPUFamilyApple3)) return 16384;
        return 8192;
    }

}
