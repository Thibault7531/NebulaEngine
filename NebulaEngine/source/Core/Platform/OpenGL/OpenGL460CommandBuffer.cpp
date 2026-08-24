#include "OpenGL460CommandBuffer.h"

#include "OpenGL460RenderPass.h"

namespace Nebula::Graphics
{
    Ref<RenderPass> OpenGL460CommandBuffer::CreateRenderPass(Ref<Framebuffer> framebuffer, bool clear)
    {
        return CreateRef<OpenGL460RenderPass>(framebuffer, clear);
    }
}
