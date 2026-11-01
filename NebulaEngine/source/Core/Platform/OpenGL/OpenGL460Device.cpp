#include "OpenGL460Device.h"

#include "OpenGL460CommandBuffer.h"
#include "OpenGL460Framebuffer.h"
#include "OpenGL460ShaderPipeline.h"
#include "OpenGL460Texture.h"
#include "OpenGLBuffer.h"
#include "OpenGLShader.h"

namespace Nebula::Graphics
{
    Ref<VertexBuffer> OpenGL460Device::CreateVertexBuffer(uint32_t size)
    {
        return CreateRef<OpenGLVertexBuffer>(size);
    }

    Ref<VertexBuffer> OpenGL460Device::CreateVertexBuffer(const void* data, uint32_t size)
    {
        return CreateRef<OpenGLVertexBuffer>(data, size);
    }

    Ref<IndexBuffer> OpenGL460Device::CreateIndexBuffer(uint32_t count)
    {
        return CreateRef<OpenGLIndexBuffer>(count);
    }

    Ref<IndexBuffer> OpenGL460Device::CreateIndexBuffer(uint16_t* data, uint32_t count)
    {
        return CreateRef<OpenGLIndexBuffer>(data, count);
    }

    Ref<IndexBuffer> OpenGL460Device::CreateIndexBuffer(uint32_t* data, uint32_t count)
    {
        return CreateRef<OpenGLIndexBuffer>(data, count);
    }

    Ref<UniformBuffer> OpenGL460Device::CreateUniformBuffer(const BufferLayout& layout, uint32_t instances)
    {
        return CreateRef<OpenGL460UniformBuffer>(this, layout, instances);
    }

    Ref<Texture2D> OpenGL460Device::CreateTexture2D(uint32_t width, uint32_t height,
        const Texture::TextureSpecification& spec)
    {
        return CreateRef<OpenGL460Texture2D>(width, height, spec);
    }

    Ref<Texture2D> OpenGL460Device::CreateTexture2D(std::filesystem::path path,
        const Texture::TextureSpecification& spec)
    {
        return CreateRef<OpenGL460Texture2D>(path, spec);
    }

    Ref<Framebuffer> OpenGL460Device::CreateFramebuffer(const FramebufferSpecification& spec)
    {
        return CreateRef<OpenGL460Framebuffer>(spec);
    }

    Ref<Framebuffer> OpenGL460Device::CreateFramebufferForSwapchain(const FramebufferSpecification& spec,
        uint32_t swapchainTextureIndex, Ref<Texture2D> swapchainTexture) {}

    Ref<SwapChain> OpenGL460Device::CreateSwapchain(Ref<Surface> surface,
        const FramebufferSpecification& framebufferSpec, uint32_t swapchainTextureIndex) {}

    Ref<Shader> OpenGL460Device::CreateShader(const std::filesystem::path& path)
    {
        return CreateRef<OpenGLShader>(path);
    }

    Ref<ShaderPipeline> OpenGL460Device::CreateShaderPipeline(Ref<Shader> shader, VertexLayout layout,
        const PipelineSpecification& spec)
    {
        return CreateRef<OpenGL460ShaderPipeline>(std::dynamic_pointer_cast<OpenGLShader>(shader), layout, spec);
    }

    Ref<CommandBuffer> OpenGL460Device::CreateCommandBuffer()
    {
        return CreateRef<OpenGL460CommandBuffer>();
    }

    uint32_t OpenGL460Device::GetMaximumTextureSize()
    {
        int32_t size = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
        return size;
    }

    uint32_t OpenGL460Device::GetUniformBufferAlignment()
    {
        int32_t alignment = 0;
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
        return alignment;
    }

    Ref<Device> OpenGL460Device::CreateDefaultDevice()
    {
        return CreateRef<OpenGL460Device>();
    }
}
