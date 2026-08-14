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

    private:
        MTL::Device* m_Device;
        MTL::CommandQueue* m_CommandQueue;
    };

}
