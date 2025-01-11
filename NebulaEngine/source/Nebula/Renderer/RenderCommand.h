#pragma once

#include "RendererAPI.h"

namespace Nebula {
	/**
	 * Static class to send render commands.
	 */
	class RenderCommand
	{
	public:
		/**
		 * Initializes the renderer with correct settings.
		 */
		inline static void Init()
		{
			s_RendererAPI->Init();
		}

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		/**
		 * Sets the color of the background.
		 * @param color Color for the background
		 */
		inline static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}
		/**
		 * Clears the viewport.
		 */
		inline static void Clear()
		{
			s_RendererAPI->Clear();
		}

		/**
		 * Draws the specified vertex array on the screen.
		 * @param vertexArray Vertex array to be drawn on the screen
		 */
		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			s_RendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static RendererAPI* s_RendererAPI;
	};

}