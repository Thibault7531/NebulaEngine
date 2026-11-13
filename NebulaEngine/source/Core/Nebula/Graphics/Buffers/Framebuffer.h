#pragma once

#include "Nebula/Graphics/Texture.h"

namespace Nebula::Graphics
{
    struct FramebufferAttachmentSpecification
    {
        std::vector<Texture::TextureSpecification> Attachments;

        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(std::initializer_list<Texture::TextureSpecification> elements)
            : Attachments(elements) {}

        bool operator==(const FramebufferAttachmentSpecification& other) const = default;
    };

    struct FramebufferSpecification
    {
        uint32_t width, height;
        uint32_t samples = 1;

        FramebufferAttachmentSpecification AttachmentSpec = {
            Texture::InternalFormat::RGBA8, Texture::InternalFormat::Depth24Stencil8
        };

        bool SwapChainTarget = false;

        bool operator==(const FramebufferSpecification& other) const = default;
    };

    enum class ColorAttachementDataType
    {
        None,
        Int,
        UnsignedInt,
        Float
    };

    class Framebuffer
    {
    public:
        virtual ~Framebuffer() = default;

        virtual void SetClearColors(const std::vector<glm::vec4>& colors) = 0;
        virtual void SetClearDepth(float depth) = 0;
        virtual void SetClearStencil(int32_t stencil) = 0;

        virtual uint32_t GetColorAttachmentCount() const = 0;
        virtual Ref<Texture2D> GetColorAttachment(uint32_t index = 0) const = 0;
        virtual Ref<Texture2D> GetSwapchainColorAttachment() const = 0;
        virtual const std::vector<Texture::TextureSpecification>& GetColorAttachmentSpecs() const = 0;
        virtual const std::vector<glm::vec4>& GetClearColors() const = 0;
        virtual Ref<Texture2D> GetDepthAttachment() const = 0;
        virtual const Texture::TextureSpecification& GetDepthAttachmentSpec() const = 0;
        virtual float GetClearDepth() const = 0;

        [[nodiscard]] virtual const FramebufferSpecification& GetSpecification() const = 0;

        virtual void Resize(uint32_t width, uint32_t height) = 0;

    public:
        static bool IsDepthFormat(Texture::InternalFormat format);
        static bool IsStencilFormat(Texture::InternalFormat format);
        static bool IsDepthStencilFormat(Texture::InternalFormat format);

    };
}
