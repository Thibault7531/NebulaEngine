#pragma once

#include "Cameras/OrthographicCamera.h"
#include "Cameras/PerspectiveCamera.h"
#include "Model/Model.h"
#include "Nebula/Graphics/CommandBuffer.h"
#include "Nebula/Graphics/Device.h"
#include "Nebula/Graphics/RenderPass.h"
#include "Nebula/Graphics/Shader.h"
#include "Nebula/Graphics/Buffers/VertexArray.h"
#include "Nebula/Graphics/Core/RendererAPI.h"

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

		static void Submit(const Ref<Graphics::ShaderPipeline>& shader, const Ref<Graphics::VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f), const std::map<std::string, Ref<Graphics::UniformBuffer>>& uniformBuffers = {});
		static void SubmitModel(const Ref<Graphics::ShaderPipeline>& shader, const Ref<Model>& model, const glm::mat4& transform = glm::mat4(1.0f));

		inline static Platform::GraphicsAPI GetAPI() { return Graphics::RendererAPI::GetAPI(); }

	private:
		static void SetRenderState();

	private:
		struct SceneData
		{
			glm::mat4 viewProjectionMatrix;

			uint32_t m_Width, m_Height;

			Ref<Graphics::Device> m_Device;
			Ref<Graphics::CommandBuffer> m_CommandBuffer;
			Ref<Graphics::RenderPass> m_RenderPass;
			Ref<Graphics::UniformBuffer> m_UBO;
		};

		static SceneData* m_SceneData;
	};

}
