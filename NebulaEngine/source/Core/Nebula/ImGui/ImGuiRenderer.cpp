#include "ImGuiRenderer.h"

#include "Nebula/Renderer/Renderer.h"

#ifdef NB_PLATFORM_MACOS
#include "Platform/OpenGL410/OpenGL410ImGuiRenderer.h"
#else
#include "Platform/OpenGL/OpenGLImGuiRenderer.h"
#include "Platform/OpenGL410/OpenGL410ImGuiRenderer.h"
#endif

namespace Nebula
{

    Ref<ImGuiRenderer> ImGuiRenderer::Create()
    {
        switch (Renderer::GetAPI())
        {
#ifdef NB_PLATFORM_MACOS
        case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case RendererAPI::API::OpenGL: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
        case RendererAPI::API::OpenGL410: return CreateRef<OpenGL410ImGuiRenderer>();
#else
        case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLImGuiRenderer>();
        case RendererAPI::API::OpenGL410: return CreateRef<OpenGL410::OpenGL410ImGuiRenderer>();
#endif
        }

        NB_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

}
