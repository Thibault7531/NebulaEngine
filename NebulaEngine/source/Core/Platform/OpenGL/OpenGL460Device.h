#pragma once

#include "Nebula/Graphics/Device.h"

namespace Nebula::Graphics
{
    class OpenGL460Device : public Device
    {
    public:
        OpenGL460Device() = default;
        virtual ~OpenGL460Device() = default;

        virtual void SetSurfaceDevice(Ref<Surface> surface) override {}

        virtual Ref<VertexBuffer> CreateVertexBuffer(uint32_t size) override;
        virtual Ref<VertexBuffer> CreateVertexBuffer(const void* data, uint32_t size) override;

        virtual Ref<IndexBuffer> CreateIndexBuffer(uint32_t count) override;
        virtual Ref<IndexBuffer> CreateIndexBuffer(uint16_t* data, uint32_t count) override;
        virtual Ref<IndexBuffer> CreateIndexBuffer(uint32_t* data, uint32_t count) override;

        virtual Ref<UniformBuffer> CreateUniformBuffer(const BufferLayout& layout, uint32_t instances) override;

        virtual Ref<Texture2D> CreateTexture2D(uint32_t width, uint32_t height, const Texture::TextureSpecification& spec) override;
        virtual Ref<Texture2D> CreateTexture2D(std::filesystem::path path, const Texture::TextureSpecification& spec) override;

        virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification& spec) override;

        virtual Ref<Shader> CreateShader(const std::filesystem::path& path) override;
        virtual Ref<ShaderPipeline> CreateShaderPipeline(Ref<Shader> shader, VertexLayout layout, const PipelineSpecification& spec) override;

        virtual Ref<CommandBuffer> CreateCommandBuffer() override;


    public:
        virtual uint32_t GetMaximumTextureSize() override;
        virtual uint32_t GetUniformBufferAlignment() override;

    public:
        static Ref<Device> CreateDefaultDevice();
    };
}
