#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Renderer.h"

#ifdef NB_PLATFORM_MACOS
#include "Platform/OpenGL410/OpenGL410Texture.h"
#else
#include "Platform/OpenGL/OpenGL460Texture.h"
#include "Platform/OpenGL410/OpenGL410Texture.h"
#endif

namespace Nebula {

    Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, InternalFormat internalFormat)
    {
        switch (Renderer::GetAPI())
        {
#ifdef NB_PLATFORM_MACOS
        case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case Platform::GraphicsAPI::OpenGL410: return CreateRef<OpenGL410Texture2D>(width, height, internalFormat);
        case Platform::GraphicsAPI::OpenGL460: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
#else
        case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case Platform::GraphicsAPI::OpenGL410: return CreateRef<OpenGL410Texture2D>(width, height, internalFormat);
        case Platform::GraphicsAPI::OpenGL460: return CreateRef<OpenGL460Texture2D>(width, height, internalFormat);
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
        case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case Platform::GraphicsAPI::OpenGL410: return CreateRef<OpenGL410Texture2D>(path);
        case Platform::GraphicsAPI::OpenGL460: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
#else
        case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
        case Platform::GraphicsAPI::OpenGL410: return CreateRef<OpenGL410Texture2D>(path);
        case Platform::GraphicsAPI::OpenGL460: return CreateRef<OpenGL460Texture2D>(path);
#endif
        }

        NB_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

}
