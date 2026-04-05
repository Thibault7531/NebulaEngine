#include "VertexArray.h"

#include "Nebula/Renderer/Renderer.h"

#ifdef NB_PLATFORM_MACOS
#include "Platform/OpenGL410/OpenGL410VertexArray.h"
#else
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Platform/OpenGL410/OpenGL410VertexArray.h"
#endif

namespace Nebula {

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
#ifdef NB_PLATFORM_MACOS
		case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case RendererAPI::API::OpenGL: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
		case RendererAPI::API::OpenGL410: return std::make_shared<OpenGL410VertexArray>();
#else
		case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexArray>();
		case RendererAPI::API::OpenGL410: return std::make_shared<OpenGL410VertexArray>();
#endif
		}

		NB_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}


}
