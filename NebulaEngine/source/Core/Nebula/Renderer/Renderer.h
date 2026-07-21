#pragma once

#include "../Graphics/Core/RendererAPI.h"
#include "Cameras/OrthographicCamera.h"
#include "Cameras/PerspectiveCamera.h"
#include "Model/Model.h"
#include "Nebula/Graphics/Shader.h"
#include "Nebula/Graphics/Buffers/VertexArray.h"

namespace Nebula {

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(const PerspectiveCamera& camera);
		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const Ref<Graphics::Shader>& shader, const Ref<Graphics::VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));
		static void SubmitModel(const Ref<Graphics::Shader>& shader, const Ref<Model>& model, const glm::mat4& transform = glm::mat4(1.0f));

		inline static Platform::GraphicsAPI GetAPI() { return Graphics::RendererAPI::GetAPI(); }

	private:
		static void SetRenderState();

	private:
		struct SceneData
		{
			glm::mat4 viewProjectionMatrix;

			Ref<Graphics::UniformBuffer> m_UBO;
		};

		static SceneData* m_SceneData;
	};

}
