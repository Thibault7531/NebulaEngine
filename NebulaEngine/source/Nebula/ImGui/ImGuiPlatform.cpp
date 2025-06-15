#include "ImGuiPlatform.h"

#include "Platform/Windows/WindowsImGuiPlatform.h"

namespace Nebula
{

    Ref<ImGuiPlatform> ImGuiPlatform::Create()
    {
#ifdef NB_PLATFORM_WINDOWS
        return CreateRef<WindowsImGuiPlatform>();
#endif

        NB_ASSERT(false, "Unknown platform")
        return nullptr;
    }

}
