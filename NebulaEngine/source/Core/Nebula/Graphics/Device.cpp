#include "Device.h"

#include "Core/RendererAPI.h"
#include "Core/Nebula/CoreSystems/Core/Log.h"
#include "Platform/Nebula/CoreSystems/Graphics/GraphicsAPI.h"
#include "Platform/OpenGL/OpenGL460Device.h"
#ifdef NB_PLATFORM_MACOS
#include "Core/Platform/Metal3/Metal3Device.h"
#endif

namespace Nebula::Graphics
{

    Ref<Device> Device::CreateDefaultDevice()
    {
#ifdef NB_PLATFORM_WINDOWS
        switch (RendererAPI::GetAPI())
        {
        case Platform::GraphicsAPI::None: NB_CORE_ERROR("None is not supported on Windows!"); break;
        case Platform::GraphicsAPI::Metal3: NB_CORE_ERROR("Metal3 is not supported on Windows!"); break;
        case Platform::GraphicsAPI::OpenGL460: return CreateRef<OpenGL460Device>();
        }
#elifdef NB_PLATFORM_MACOS
        switch (RendererAPI::GetAPI())
        {
        case Platform::GraphicsAPI::None: NB_CORE_ERROR("None is not supported on MacOS!"); break;
        case Platform::GraphicsAPI::Metal3: return CreateRef<Metal3Device>();
        case Platform::GraphicsAPI::OpenGL460: NB_CORE_ERROR("OpenGL460 is not supported on MacOS!"); break;
        }
#elifdef NB_PLATFORM_LINUX
        switch (RendererAPI::GetAPI())
        {
        case Platform::GraphicsAPI::None: NB_CORE_ERROR("None is not supported on Linux!"); break;
        case Platform::GraphicsAPI::Metal3: NB_CORE_ERROR("Metal3 is not supported on Linux!"); break;
        case Platform::GraphicsAPI::OpenGL460: return CreateRef<OpenGL460Device>();
        }
#endif
        return nullptr;
    }

}
