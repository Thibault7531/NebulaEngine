#pragma once

#include <filesystem>

#include "CommandBuffer.h"
#include "Queue.h"
#include "ShaderModule.h"
#include "SwapChain.h"
#include "Texture.h"
#include "Buffers/Buffer.h"
#include "Buffers/Framebuffer.h"
#include "Core/Nebula/CoreSystems/Core/Core.h"
#include "Platform/Nebula/CoreSystems/Graphics/Surface.h"

namespace Nebula::Graphics
{
    inline void HashCombine(size_t& seed, size_t value)
    {
        seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
    }

    template<typename T>
    inline void HashCombineValue(size_t& seed, const T& value)
    {
        HashCombine(seed, std::hash<T>{}(value));
    }

    inline size_t Hash(const Stage& stage)
    {
        size_t hash = 0;
        HashCombineValue(hash, stage.ShaderStage);
        HashCombineValue(hash, stage.Module);
        return hash;
    }

    inline size_t Hash(const BlendState& blendState)
    {
        size_t hash = 0;
        HashCombineValue(hash, blendState.enabled);
        HashCombineValue(hash, blendState.srcColorFactor);
        HashCombineValue(hash, blendState.dstColorFactor);
        HashCombineValue(hash, blendState.colorOperation);
        HashCombineValue(hash, blendState.srcAlphaFactor);
        HashCombineValue(hash, blendState.dstAlphaFactor);
        HashCombineValue(hash, blendState.alphaOperation);
        return hash;
    }

    inline size_t Hash(const FramebufferSpecification& framebufferSpec)
    {
        size_t hash = 0;
        HashCombineValue(hash, framebufferSpec.width);
        HashCombineValue(hash, framebufferSpec.height);
        HashCombineValue(hash, framebufferSpec.samples);
        for (const auto& attachment : framebufferSpec.AttachmentSpec.Attachments)
        {
            HashCombineValue(hash, attachment.InternalFormat);
            HashCombineValue(hash, attachment.MinFilter);
            HashCombineValue(hash, attachment.MagFilter);
        }
        HashCombineValue(hash, framebufferSpec.SwapChainTarget);
        return hash;
    }

    struct PipelineSpecificationHash
    {
        size_t operator()(const PipelineSpecification& spec) const
        {
            size_t hash = 0;
            for (const auto& stage : spec.stages)
            {
                HashCombine(hash, Hash(stage));
            }
            HashCombine(hash, Hash(spec.blendState));
            HashCombine(hash, Hash(spec.framebufferSpec));
            return hash;
        }
    };

    class Device
    {
    public:
        virtual ~Device() = default;

        virtual void SetSurfaceDevice(Ref<Surface> surface) = 0;

        virtual void WaitIdle() = 0;

        virtual Ref<VertexBuffer> CreateVertexBuffer(uint32_t size) = 0;
        virtual Ref<VertexBuffer> CreateVertexBuffer(const void* data, uint32_t size) = 0;

        virtual Ref<IndexBuffer> CreateIndexBuffer(uint32_t count) = 0;
        virtual Ref<IndexBuffer> CreateIndexBuffer(uint16_t* data, uint32_t count) = 0;
        virtual Ref<IndexBuffer> CreateIndexBuffer(uint32_t* data, uint32_t count) = 0;

        virtual Ref<UniformBuffer> CreateUniformBuffer(const BufferLayout& layout, uint32_t instances = 1) = 0;

        virtual Ref<Texture2D> CreateTexture2D(uint32_t width, uint32_t height, const Texture::TextureSpecification& spec) = 0;
        virtual Ref<Texture2D> CreateTexture2D(std::filesystem::path path, const Texture::TextureSpecification& spec) = 0;

        virtual Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification& spec) = 0;
        virtual Ref<Framebuffer> CreateFramebufferForSwapchain(const FramebufferSpecification& spec, uint32_t swapchainTextureIndex, Ref<Texture2D> swapchainTexture) = 0;

        virtual Ref<SwapChain> CreateSwapchain(Ref<Surface> surface, const FramebufferSpecification& framebufferSpec, uint32_t swapchainTextureIndex) = 0;

        virtual Ref<ShaderModule> CreateShaderModule(const std::filesystem::path& path) = 0;
        virtual Ref<ShaderPipeline> CreateShaderPipeline(const VertexLayout& layout,  const PipelineSpecification& spec) = 0;

    public:
        virtual uint32_t GetMaximumTextureSize() = 0;
        virtual uint32_t GetUniformBufferAlignment() = 0;

        virtual Ref<Queue> GetGraphicsQueue() = 0;

    public:
        static Ref<Device> CreateDefaultDevice();
    };

}
