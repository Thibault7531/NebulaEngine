#include "VertexArray.h"

#include "Nebula/Graphics/Core/RendererAPI.h"
#include "Platform/Metal3/Metal3VertexArray.h"
#include "Platform/Vulkan/VulkanVertexArray.h"

namespace Nebula::Graphics
{
    Ref<VertexArray> VertexArray::Create()
    {
#ifdef NB_PLATFORM_WINDOWS
        switch (RendererAPI::GetAPI())
        {
        case Platform::GraphicsAPI::None: NB_CORE_ERROR("None is not supported on Windows!"); break;
        case Platform::GraphicsAPI::Metal3: NB_CORE_ERROR("Metal3 is not supported on Windows!"); break;
        case Platform::GraphicsAPI::Vulkan: return CreateRef<VulkanVertexArray>();
        }
#elifdef NB_PLATFORM_MACOS
        switch (RendererAPI::GetAPI())
        {
        case Platform::GraphicsAPI::None: NB_CORE_ERROR("None is not supported on MacOS!"); break;
        case Platform::GraphicsAPI::Metal3: return CreateRef<Metal3VertexArray>();
        case Platform::GraphicsAPI::Vulkan: NB_CORE_ERROR("Vulkan is not supported on MacOS!"); break;
        }
#elifdef  NB_PLATFORM_LINUX
        switch (RendererAPI::GetAPI())
        {
        case Platform::GraphicsAPI::None: NB_CORE_ERROR("None is not supported on Linux!"); break;
        case Platform::GraphicsAPI::Metal3: NB_CORE_ERROR("Metal3 is not supported on Linux!"); break;
        case Platform::GraphicsAPI::Vulkan: return CreateRef<VulkanVertexArray>();
        }
#endif
        return nullptr;
    }
}
