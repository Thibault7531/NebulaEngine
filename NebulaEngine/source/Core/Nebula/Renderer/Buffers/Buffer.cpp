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
		case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case Platform::GraphicsAPI::OpenGL410: return std::make_shared<OpenGL410VertexBuffer>(size);
		case Platform::GraphicsAPI::OpenGL460: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
#else
		case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case Platform::GraphicsAPI::OpenGL410: return std::make_shared<OpenGL410VertexBuffer>(size);
		case Platform::GraphicsAPI::OpenGL460: return std::make_shared<OpenGLVertexBuffer>(size);
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
		case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case Platform::GraphicsAPI::OpenGL410: return std::make_shared<OpenGL410VertexBuffer>(vertices, size);
		case Platform::GraphicsAPI::OpenGL460: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
#else
		case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case Platform::GraphicsAPI::OpenGL410: return std::make_shared<OpenGL410VertexBuffer>(vertices, size);
		case Platform::GraphicsAPI::OpenGL460: return std::make_shared<OpenGLVertexBuffer>(vertices, size);
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
		case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case Platform::GraphicsAPI::OpenGL410: return std::make_shared<OpenGL410IndexBuffer>(indices, count);
		case Platform::GraphicsAPI::OpenGL460: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
#else
		case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case Platform::GraphicsAPI::OpenGL410: return std::make_shared<OpenGL410IndexBuffer>(indices, count);
		case Platform::GraphicsAPI::OpenGL460: return std::make_shared<OpenGLIndexBuffer>(indices, count);
#endif
		}

		NB_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
