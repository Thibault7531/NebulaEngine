#pragma once

#include "Shader.h"
#include "Buffers/VertexArray.h"
#include "Cameras/OrthographicCamera.h"
#include "Cameras/PerspectiveCamera.h"
#include "Core/RendererAPI.h"
#include "Model/Model.h"

namespace Nebula {

	class Renderer
	{
	public:
		static void Init(Platform::GraphicsAPI api);
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const PerspectiveCamera& camera);
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitModel(const Ref<Shader>& shader, const Ref<Model>& model, const glm::mat4& transform = glm::mat4(1.0f));

		inline static Platform::GraphicsAPI GetAPI() { return RendererAPI::GetAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 viewProjectionMatrix;
		};

		static SceneData* m_SceneData;
	};

}
