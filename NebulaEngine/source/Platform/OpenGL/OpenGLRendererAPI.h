#pragma once

#include "Nebula/Renderer/RendererAPI.h"

namespace Nebula {
	/**
	 * OpenGL API.
	 */
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
	};

}