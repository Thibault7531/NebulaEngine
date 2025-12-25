#include "Log.h"

#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Nebula::Platform
{
    std::shared_ptr<spdlog::logger> Log::s_PlatformLogger;

    void Log::Init()
    {
        spdlog::set_pattern("%^[%Y/%m/%d %T] %n: [%l] %v%$");

        s_PlatformLogger = spdlog::stdout_color_mt("PLATFORM");
        s_PlatformLogger->set_level(spdlog::level::trace);
    }

}
