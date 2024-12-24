#pragma once

#include <memory>

/* Windows specific */
#ifdef NB_PLATFORM_WINDOWS
#else
	#error Nebula Engine only supports windows
#endif // NB_PLATFORM_WINDOWS

#ifdef NB_DEBUG
	#define NB_ENABLE_ASSERTS
#endif

/* Debug */
#ifdef NB_ENABLE_ASSERTS
	#define NB_ASSERT(x, ...) { if (!(x)) { NB_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define NB_CORE_ASSERT(x, ...) { if (!(x)) { NB_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define NB_ASSERT(x, ...)
	#define NB_CORE_ASSERT(x, ...)
#endif // NB_ENABLE_ASSERTS

#define  BIT(x) (1 << x)

#define NB_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Nebula
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

}
