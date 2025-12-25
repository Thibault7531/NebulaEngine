#pragma once

namespace Nebula::Platform
{

    class CoreUtils
    {
    public:
        static void InitThread();
        static void InitThread_MT();

        static void PumpEvents();
    };

}
