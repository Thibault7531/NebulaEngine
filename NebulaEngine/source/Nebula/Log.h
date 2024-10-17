#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Nebula {

	class NEBULA_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; };
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; };

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define NB_CORE_TRACE(...)      ::Nebula::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define NB_CORE_DEBUG(...)      ::Nebula::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define NB_CORE_INFO(...)       ::Nebula::Log::GetCoreLogger()->info(__VA_ARGS__)
#define NB_CORE_WARN(...)       ::Nebula::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define NB_CORE_ERROR(...)      ::Nebula::Log::GetCoreLogger()->error(__VA_ARGS__)
#define NB_CORE_CRITICAL(...)   ::Nebula::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define NB_TRACE(...)           ::Nebula::Log::GetClientLogger()->trace(__VA_ARGS__)
#define NB_DEBUG(...)           ::Nebula::Log::GetClientLogger()->debug(__VA_ARGS__)
#define NB_INFO(...)            ::Nebula::Log::GetClientLogger()->info(__VA_ARGS__)
#define NB_WARN(...)            ::Nebula::Log::GetClientLogger()->warn(__VA_ARGS__)
#define NB_ERROR(...)           ::Nebula::Log::GetClientLogger()->error(__VA_ARGS__)
#define NB_CRITICAL(...)        ::Nebula::Log::GetClientLogger()->critical(__VA_ARGS__)