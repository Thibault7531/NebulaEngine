#include "Framebuffer.h"

namespace Nebula::Graphics
{

    bool Framebuffer::IsDepthFormat(Texture::InternalFormat format)
    {
        switch (format)
        {
        default: return false;
        }
    }

    bool Framebuffer::IsStencilFormat(Texture::InternalFormat format)
    {
        switch (format)
        {
        default: return false;
        }
    }

    bool Framebuffer::IsDepthStencilFormat(Texture::InternalFormat format)
    {
        switch (format)
        {
        case Texture::InternalFormat::Depth24Stencil8: return true;
        case Texture::InternalFormat::Depth32Stencil8: return true;
        default: return false;
        }
    }
}
