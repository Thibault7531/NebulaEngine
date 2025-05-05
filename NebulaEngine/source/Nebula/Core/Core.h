#pragma once

#include <memory>

// Platform detection
#ifdef _WIN32
	/* Windows x64/x86 */
	#ifdef _WIN64
		/* Windows x64 */
		#define NB_PLATFORM_WINDOWS
	#else
		/* Windows x86 */
		#error "x86 Builds are not supported"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported"
	#elif TARGET_OS_IPHONE == 1
		#define NB_PLATFORM_IOS
		#error "IOS is not supported"
	#elif TARGET_OS_MAC == 1
		#define NB_PLATFORM_MACOS
		#error "MacOS is not supported"
	#else
		#error "Unknown Apple platform"
	#endif
#elif defined(__ANDROID__)
	#define NB_PLATFORM_ANDROID
	#error "Android is not supported"
#elif defined(__linux__)
	#define NB_PLATFORM_LINUX
	#error "Linux is not supported"
#else
	#error "Unknown platform"
#endif


// Debug
#ifdef NB_ENGINE_DEBUG
	#define NB_ENABLE_ASSERTS
#endif

#ifdef NB_ENABLE_ASSERTS
	#define NB_ASSERT(x, ...) { if (!(x)) { NB_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define NB_CORE_ASSERT(x, ...) { if (!(x)) { NB_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define NB_ASSERT(x, ...)
	#define NB_CORE_ASSERT(x, ...)
#endif // NB_ENABLE_ASSERTS



#define  BIT(x) (1 << x)

#define NB_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Nebula {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}
