#include "Device.h"

#include "Core/RendererAPI.h"
#include "Core/Nebula/CoreSystems/Core/Log.h"
#include "Core/Platform/Metal3/Metal3Device.h"
#include "Platform/Nebula/CoreSystems/Graphics/GraphicsAPI.h"

namespace Nebula::Graphics
{

    Ref<Device> Device::CreateDefaultDevice()
    {
#ifdef NB_PLATFORM_MACOS
        switch (RendererAPI::GetAPI())
        {
        case Platform::GraphicsAPI::None: NB_CORE_ERROR("None is not supported on MacOS!"); break;
        case Platform::GraphicsAPI::Metal3: return CreateRef<Metal3Device>();
        case Platform::GraphicsAPI::OpenGL460: NB_CORE_ERROR("OpenGL460 is not supported on MacOS!"); break;
        }
#endif
    }

}
