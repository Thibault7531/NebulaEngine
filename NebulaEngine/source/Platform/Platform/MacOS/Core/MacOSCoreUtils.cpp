#include "Platform/Nebula/CoreSystems/Core/CoreUtils.h"

#include "SwiftCoreUtils.h"

namespace Nebula::Platform
{
    void CoreUtils::InitThread()
    {
        SwiftCoreUtils::InitThread();
    }

    void CoreUtils::InitThread_MT()
    {
    }

    void CoreUtils::PumpEvents()
    {
    }
}
