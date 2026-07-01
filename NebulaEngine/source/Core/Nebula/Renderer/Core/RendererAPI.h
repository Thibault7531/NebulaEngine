#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "Nebula/Renderer/Buffers/VertexArray.h"
#include "Platform/Nebula/CoreSystems/Rendering/GraphicsAPI.h"

namespace Nebula {

	class RendererAPI
	{
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void Log() = 0;

		virtual bool IsFaceCullingEnabled() const = 0;
		virtual void EnableFaceCulling(bool enabled) = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		inline static Platform::GraphicsAPI GetAPI() { return s_API; }

	private:
		static Platform::GraphicsAPI s_API;

		friend class RenderCommand;
	};

}
