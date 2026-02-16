#pragma once

#include <memory>
#include <signal.h>


// Debug
#ifdef NB_ENGINE_DEBUG
	#define NB_ENABLE_ASSERTS
#endif

#ifdef NB_ENABLE_ASSERTS
	#ifdef NB_PLATFORM_WINDOWS
		#define NB_ASSERT(x, ...) { if (!(x)) { NB_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
		#define NB_CORE_ASSERT(x, ...) { if (!(x)) { NB_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#else
		#define NB_ASSERT(x, ...) { if (!(x)) { NB_ERROR("Assertion Failed: {0}", ##__VA_ARGS__); raise(SIGTRAP); } }
		#define NB_CORE_ASSERT(x, ...) { if (!(x)) { NB_CORE_ERROR("Assertion Failed: {0}", ##__VA_ARGS__); raise(SIGTRAP); } }
	#endif
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