#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Renderer.h"

#ifdef NB_PLATFORM_MACOS
#include "Platform/OpenGL410/OpenGL410Texture.h"
#else
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/OpenGL410/OpenGL410Texture.h"
#endif

namespace Nebula {

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
    {
        switch (Renderer::GetAPI())
        {
#ifdef NB_PLATFORM_MACOS
        case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case RendererAPI::API::OpenGL: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
        case RendererAPI::API::OpenGL410: return CreateRef<OpenGL410Texture2D>(width, height);
#else
        case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width, height);
        case RendererAPI::API::OpenGL410: return CreateRef<OpenGL410Texture2D>(width, height);
#endif
        }

        NB_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

    Ref<Texture2D> Texture2D::Create(const std::string& path)
    {
        switch (Renderer::GetAPI())
        {
#ifdef NB_PLATFORM_MACOS
        case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case RendererAPI::API::OpenGL: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
        case RendererAPI::API::OpenGL410: return CreateRef<OpenGL410Texture2D>(path);
#else
        case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path);
        case RendererAPI::API::OpenGL410: return CreateRef<OpenGL410Texture2D>(path);
#endif
        }

        NB_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

}
