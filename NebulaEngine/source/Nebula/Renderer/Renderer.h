#pragma once

#include "OrthographicCamera.h"
#include "RendererAPI.h"
#include "Shader.h"

namespace Nebula {
	/**
	 * Static class to send commands to the renderer.
	 */
	class Renderer
	{
	public:
		/**
		 * Initializes the renderer with the correct settings.
		 */
		static void Init();

		/**
		 * Begins the rendering process.
		 */
		static void BeginScene(OrthographicCamera& camera);
		/**
		 * Ends the rendering process.
		 */
		static void EndScene();

		/**
		 * Submits a vertex array to be drawn on the screen.
		 * @param shader Shader used to draw Vertex array
		 * @param vertexArray Vertex array to be drawn on the screen
		 * @param transform Transform matrix to move the object
		 */
		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		/**
		 * Gets the Graphics API being used.
		 * @return Graphics API
		 */
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 viewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};

}
