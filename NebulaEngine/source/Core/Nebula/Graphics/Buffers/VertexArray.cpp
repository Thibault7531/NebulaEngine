#include "VertexArray.h"

#include "Nebula/Graphics/Core/RendererAPI.h"
#include "Platform/Metal3/Metal3VertexArray.h"

namespace Nebula::Graphics
{
    Ref<VertexArray> VertexArray::Create()
    {
#ifdef NB_PLATFORM_MACOS
        switch (RendererAPI::GetAPI())
        {
        case Platform::GraphicsAPI::None: NB_CORE_ERROR("None is not supported on MacOS!"); break;
        case Platform::GraphicsAPI::Metal3: return CreateRef<Metal3VertexArray>();
        case Platform::GraphicsAPI::OpenGL460: NB_CORE_ERROR("OpenGL460 is not supported on MacOS!"); break;
        }
#endif
    }
}
