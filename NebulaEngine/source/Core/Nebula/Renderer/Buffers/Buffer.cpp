#include "Buffer.h"

#include "Nebula/Renderer/Renderer.h"

#ifdef NB_PLATFORM_MACOS
#include "Platform/OpenGL410/OpenGL410Buffer.h"
#else
#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/OpenGL410/OpenGL410Buffer.h"
#endif

namespace Nebula {
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
#ifdef NB_PLATFORM_MACOS
		case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case RendererAPI::API::OpenGL: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
		case RendererAPI::API::OpenGL410: return std::make_shared<OpenGL410VertexBuffer>(size);
#else
		case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(size);
		case RendererAPI::API::OpenGL410: return std::make_shared<OpenGL410VertexBuffer>(size);
#endif
		}

		NB_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
#ifdef NB_PLATFORM_MACOS
		case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case RendererAPI::API::OpenGL: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
		case RendererAPI::API::OpenGL410: return std::make_shared<OpenGL410VertexBuffer>(vertices, size);
#else
		case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		case RendererAPI::API::OpenGL410: return std::make_shared<OpenGL410VertexBuffer>(vertices, size);
#endif
		}

		NB_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
#ifdef NB_PLATFORM_MACOS
			case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
			case RendererAPI::API::OpenGL: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
			case RendererAPI::API::OpenGL410: return std::make_shared<OpenGL410IndexBuffer>(indices, count);
#else
			case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLIndexBuffer>(indices, count);
			case RendererAPI::API::OpenGL410: return std::make_shared<OpenGL410IndexBuffer>(indices, count);
#endif
		}

		NB_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
