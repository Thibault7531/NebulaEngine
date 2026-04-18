#include "Framebuffer.h"

#include "Nebula/Renderer/Renderer.h"

#ifdef NB_PLATFORM_MACOS
#include "Platform/OpenGL410/OpenGL410Framebuffer.h"
#else
#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Platform/OpenGL410/OpenGL410Framebuffer.h"
#endif

namespace Nebula {

    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &spec)
    {
        switch (Renderer::GetAPI())
        {
#ifdef NB_PLATFORM_MACOS
        case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case RendererAPI::API::OpenGL: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
        case RendererAPI::API::OpenGL410: return CreateRef<OpenGL410Framebuffer>(spec);
#else
        case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(spec);
        case RendererAPI::API::OpenGL410: return CreateRef<OpenGL410Framebuffer>(spec);
#endif
        }

        NB_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

}
