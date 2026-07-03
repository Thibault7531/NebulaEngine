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

		inline static bool IsFaceCullingEnabled()
		{
			return s_RendererAPI->IsFaceCullingEnabled();
		}

		inline static void EnableFaceCulling(bool enabled)
		{
			s_RendererAPI->EnableFaceCulling(enabled);
		}

		inline static bool IsDepthTestEnabled()
		{
			return s_RendererAPI->IsDepthTestEnabled();
		}

		inline static void EnableDepthTest(bool enabled)
		{
			s_RendererAPI->EnableDepthTest(enabled);
		}

		inline static bool IsStencilTestEnabled()
		{
			return s_RendererAPI->IsStencilTestEnabled();
		}

		inline static void EnableStencilTest(bool enabled)
		{
			s_RendererAPI->EnableStencilTest(enabled);
		}

		inline static bool IsScissorTestEnabled()
		{
			return s_RendererAPI->IsScissorTestEnabled();
		}

		inline static void EnableScissorTest(bool enabled)
		{
			s_RendererAPI->EnableScissorTest(enabled);
		}

		inline static void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetScissor(x, y, width, height);
		}

		inline static bool IsPrimitiveRestartEnabled()
		{
			return s_RendererAPI->IsPrimitiveRestartEnabled();
		}

		inline static void EnablePrimitiveRestart(bool enabled)
		{
			s_RendererAPI->EnablePrimitiveRestart(enabled);
		}

		inline static void SetBlendingState(const RendererAPI::BlendState& state)
		{
			s_RendererAPI->SetBlendingState(state);
		}

		inline static RendererAPI::BlendState GetBlendingState()
		{
			return s_RendererAPI->GetBlendingState();
		}

		inline static uint32_t GetMaxTextureSize()
		{
			return s_RendererAPI->GetMaxTextureSize();
		}

		inline static RendererAPI::PolygonMode GetPolygonMode()
		{
			return s_RendererAPI->GetPolygonMode();
		}

		inline static void SetPolygonMode(RendererAPI::PolygonMode mode)
		{
			s_RendererAPI->SetPolygonMode(mode);
		}

		inline static RendererAPI::ClipOrigin GetClipOrigin()
		{
			return s_RendererAPI->GetClipOrigin();
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

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount, RendererAPI::DataType dataType, uint32_t offset)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount, dataType, offset);
		}

		inline static void DrawIndexedVertexOffset(const Ref<VertexArray>& vertexArray, uint32_t indexCount, RendererAPI::DataType dataType, uint32_t offset, int32_t vtxOffset)
		{
			s_RendererAPI->DrawIndexedVertexOffset(vertexArray, indexCount, dataType, offset, vtxOffset);
		}
	private:
		inline static RendererAPI* s_RendererAPI;
	};

}
