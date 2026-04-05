#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "Nebula/Renderer/Buffers/VertexArray.h"

namespace Nebula {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1, OpenGL410 = 2
		};

	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};

}
