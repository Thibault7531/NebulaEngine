#include "Framebuffer.h"

#include "Nebula/Renderer/Renderer.h"

#ifdef NB_PLATFORM_MACOS
#include "Platform/OpenGL410/OpenGL410Framebuffer.h"
#else
#include "Platform/OpenGL/OpenGL460Framebuffer.h"
#include "Platform/OpenGL410/OpenGL410Framebuffer.h"
#endif

namespace Nebula {

    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification &spec)
    {
        switch (Renderer::GetAPI())
        {
#ifdef NB_PLATFORM_MACOS
        case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case Platform::GraphicsAPI::OpenGL410: return CreateRef<OpenGL410Framebuffer>(spec);
        case Platform::GraphicsAPI::OpenGL460: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
#else
        case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case Platform::GraphicsAPI::OpenGL410: return CreateRef<OpenGL410Framebuffer>(spec);
        case Platform::GraphicsAPI::OpenGL460: return CreateRef<OpenGL460Framebuffer>(spec);
#endif
        }

        NB_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

}
