#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Nebula {
	/**
	 * Class representing the rendering API.
	 */
	class RendererAPI
	{
	public:
		/**
		 * Class describing every graphics API.
		 */
		enum class API
		{
			None = 0, OpenGL = 1
		};

	public:
		/**
		 * Sets the background color.
		 * @param color Background color
		 */
		virtual void SetClearColor(const glm::vec4& color) = 0;
		/**
		 * Clears the viewport.
		 */
		virtual void Clear() = 0;

		/**
		 * Draws the specified vertex array on the screen.
		 * @param vertexArray Vertex array to be drawn on the screen.
		 */
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;

		/**
		 * Gets the Graphics API being used.
		 * @return Graphics API
		 */
		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};

}
