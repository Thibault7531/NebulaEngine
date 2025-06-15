#include "ImGuiRenderer.h"

#include "Nebula/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLImGuiRenderer.h"

namespace Nebula
{

    Ref<ImGuiRenderer> ImGuiRenderer::Create()
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
            case RendererAPI::API::OpenGL: return CreateRef<OpenGLImGuiRenderer>();
        }

        NB_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

}
