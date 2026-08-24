#pragma once

#include <filesystem>

#include "CommandBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Buffers/Buffer.h"
#include "Buffers/Framebuffer.h"
#include "Core/Nebula/CoreSystems/Core/Core.h"
#include "Platform/Nebula/CoreSystems/Graphics/Surface.h"

namespace Nebula::Graphics
{

    class Device
    {
    public:
        virtual ~Device() = default;

        virtual void SetSurfaceDevice(Ref<Surface> surface) = 0;

        virtual Ref<VertexBuffer> CreateVertexBuffer(uint32_t size) = 0;
        virtual Ref<VertexBuffer> CreateVertexBuffer(const void* data, uint32_t size) = 0;

        virtual Ref<IndexBuffer> CreateIndexBuffer(uint32_t count) = 0;
        virtual Ref<IndexBuffer> CreateIndexBuffer(uint16_t* data, uint32_t count) = 0;
        virtual Ref<IndexBuffer> CreateIndexBuffer(uint32_t* data, uint32_t count) = 0;

        virtual Ref<UniformBuffer> CreateUniformBuffer(const BufferLayout& layout, uint32_t instances = 1) = 0;

        virtual Ref<Texture2D> CreateTexture2D(uint32_t width, uint32_t height, const Texture::TextureSpecification& spec) = 0;
        virtual Ref<Texture2D> CreateTexture2D(std::filesystem::path path, const Texture::TextureSpecification& spec) = 0;

        virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification& spec) = 0;

        virtual Ref<Shader> CreateShader(const std::filesystem::path& path) = 0;
        virtual Ref<ShaderPipeline> CreateShaderPipeline(Ref<Shader> shader, VertexLayout layout,  const PipelineSpecification& spec) = 0;

        virtual Ref<CommandBuffer> CreateCommandBuffer() = 0;

    public:
        virtual uint32_t GetMaximumTextureSize() = 0;

    public:
        static Ref<Device> CreateDefaultDevice();
    };

}
