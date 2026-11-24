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
#elifdef NB_PLATFORM_LINUX
        case Platform::GraphicsAPI::None:
            {
                NB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
                return;
            }

        case Platform::GraphicsAPI::Metal3: NB_CORE_ASSERT(false, "RendererAPI::Metal3 is not supported on Linux!"); return;
#endif
        }
    }

    Platform::GraphicsAPI RendererAPI::s_API = Platform::GraphicsAPI::None;

}
