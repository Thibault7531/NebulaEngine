#pragma once

#include <spdlog/logger.h>

namespace Nebula::Platform
{

    class Log
    {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger> GetPlatformLogger() { return s_PlatformLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_PlatformLogger;
    };

}

#ifdef NB_ENGINE_DEBUG
#define NB_PLATFORM_TRACE(...)           ::Nebula::Platform::Log::GetPlatformLogger()->trace(__VA_ARGS__)
#define NB_PLATFORM_DEBUG(...)           ::Nebula::Platform::Log::GetPlatformLogger()->debug(__VA_ARGS__)
#define NB_PLATFORM_INFO(...)            ::Nebula::Platform::Log::GetPlatformLogger()->info(__VA_ARGS__)
#define NB_PLATFORM_WARN(...)            ::Nebula::Platform::Log::GetPlatformLogger()->warn(__VA_ARGS__)
#define NB_PLATFORM_ERROR(...)           ::Nebula::Platform::Log::GetPlatformLogger()->error(__VA_ARGS__)
#define NB_PLATFORM_CRITICAL(...)        ::Nebula::Platform::Log::GetPlatformLogger()->critical(__VA_ARGS__)
#else
#define NB_PLATFORM_TRACE(...)           ::Nebula::Platform::Log::GetPlatformLogger()->trace(__VA_ARGS__)
#define NB_PLATFORM_DEBUG(...)
#define NB_PLATFORM_INFO(...)            ::Nebula::Platform::Log::GetPlatformLogger()->info(__VA_ARGS__)
#define NB_PLATFORM_WARN(...)            ::Nebula::Platform::Log::GetPlatformLogger()->warn(__VA_ARGS__)
#define NB_PLATFORM_ERROR(...)           ::Nebula::Platform::Log::GetPlatformLogger()->error(__VA_ARGS__)
#define NB_PLATFORM_CRITICAL(...)        ::Nebula::Platform::Log::GetPlatformLogger()->critical(__VA_ARGS__)
#endif
