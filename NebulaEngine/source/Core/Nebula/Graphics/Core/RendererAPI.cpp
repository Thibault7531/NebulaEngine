#include "RendererAPI.h"

#include "Nebula/CoreSystems/Core/Core.h"

namespace Nebula::Graphics {

    void RendererAPI::Init(Platform::GraphicsAPI api)
    {
        RendererAPI::s_API = api;

        switch (api)
        {
#ifdef NB_PLATFORM_MACOS
        case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return;
        case Platform::GraphicsAPI::Metal3: s_API = api; return;
        case Platform::GraphicsAPI::OpenGL410: NB_CORE_ASSERT(false, "RendererAPI::OpenGL410 not supported on MacOS"); return;
        case Platform::GraphicsAPI::OpenGL460: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return;
#else
        case Platform::GraphicsAPI::None:
            {
                NB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
                return;
            }

        case Platform::GraphicsAPI::OpenGL410: s_RendererAPI = new OpenGL410RendererAPI(); break;
        case Platform::GraphicsAPI::OpenGL460: s_RendererAPI = new OpenGLRendererAPI(); break;
#endif
        }
    }

    Platform::GraphicsAPI RendererAPI::s_API = Platform::GraphicsAPI::None;

}
