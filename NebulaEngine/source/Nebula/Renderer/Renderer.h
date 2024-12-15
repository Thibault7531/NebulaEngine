#pragma once

#include "RendererAPI.h"

namespace Nebula {
	/**
	 * Static class to send commands to the renderer.
	 */
	class Renderer
	{
	public:
		/**
		 * Begins the rendering process.
		 */
		static void BeginScene();
		/**
		 * Ends the rendering process.
		 */
		static void EndScene();

		/**
		 * Submits a vertex array to be drawn on the screen.
		 * @param vertexArray Vertex array to be drawn on the screen
		 */
		static void Submit(const std::shared_ptr<VertexArray>& vertexArray);

		/**
		 * Gets the Graphics API being used.
		 * @return Graphics API
		 */
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	};

}
