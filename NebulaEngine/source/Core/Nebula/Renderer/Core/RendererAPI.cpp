#include "RendererAPI.h"
#include "RendererAPI.h"

namespace Nebula {

#ifdef NB_PLATFORM_MACOS
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL410;
#else
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
#endif

}