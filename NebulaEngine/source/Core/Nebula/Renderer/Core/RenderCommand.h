#pragma once

#include "RendererAPI.h"
#include "Platform/OpenGL410/OpenGL410RendererAPI.h"

#ifndef NB_PLATFORM_MACOS
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#endif

namespace Nebula {

	class RenderCommand
	{
	public:
		inline static void Init(Platform::GraphicsAPI api)
		{
			RendererAPI::s_API = api;

			switch (api)
			{
#ifdef NB_PLATFORM_MACOS
			case Platform::GraphicsAPI::None:
				{
					NB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
					return;
				}

			case Platform::GraphicsAPI::OpenGL410: s_RendererAPI = new OpenGL410RendererAPI(); break;
			case Platform::GraphicsAPI::OpenGL460: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return;
#else
			case Platform::GraphicsAPI::None:
				{
					NB_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
					return;
				}

			case Platform::GraphicsAPI::OpenGL410: s_RendererAPI = new OpenGL410RendererAPI(); break;
			case Platform::GraphicsAPI::OpenGL460: s_RendererAPI = new OpenGLRendererAPI(); break;
#endif
			}
			s_RendererAPI->Init();
		}

		inline static void Log()
		{
			s_RendererAPI->Log();
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}
	private:
		inline static RendererAPI* s_RendererAPI;
	};

}
