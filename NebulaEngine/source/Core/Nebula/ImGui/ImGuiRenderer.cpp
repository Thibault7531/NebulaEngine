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
        case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case Platform::GraphicsAPI::OpenGL410: return CreateRef<OpenGL410::OpenGL410ImGuiRenderer>();
        case Platform::GraphicsAPI::OpenGL460: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
#else
        case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case Platform::GraphicsAPI::OpenGL410: return CreateRef<OpenGL410::OpenGL410ImGuiRenderer>();
        case Platform::GraphicsAPI::OpenGL460: return CreateRef<OpenGLImGuiRenderer>();
#endif
        }

        NB_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

}
