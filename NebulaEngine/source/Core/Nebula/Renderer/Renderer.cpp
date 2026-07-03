#include "Renderer.h"

#include "Renderer2D.h"
#include "Core/RenderCommand.h"

namespace Nebula {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData();

	void Renderer::Init(Platform::GraphicsAPI api)
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("Initializing Nebula Renderer");

		RenderCommand::Init(api);

		RendererAPI::BlendState state;
		state.enabled = true;
		state.srcColorFactor = RendererAPI::BlendFactor::SourceAlpha;
		state.dstColorFactor = RendererAPI::BlendFactor::OneMinusSourceAlpha;
		state.srcAlphaFactor = RendererAPI::BlendFactor::SourceAlpha;
		state.dstAlphaFactor = RendererAPI::BlendFactor::OneMinusSourceAlpha;
		RenderCommand::SetBlendingState(state);
		RenderCommand::EnableFaceCulling(true);
		RenderCommand::EnableDepthTest(true);
		RenderCommand::EnableScissorTest(false);

		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("Shutting down Nebula Renderer");
		Renderer2D::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const PerspectiveCamera& camera)
	{
		m_SceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::BeginScene(const OrthographicCamera& camera)
	{
		RenderCommand::SetBlendingState({.enabled = true});
		RenderCommand::EnableFaceCulling(true);
		RenderCommand::EnableDepthTest(true);
		RenderCommand::EnableScissorTest(false);

		m_SceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", m_SceneData->viewProjectionMatrix);
		shader->SetMat4("u_Transform", transform);

		vertexArray->Bind();

		RenderCommand::DrawIndexed(vertexArray);

		vertexArray->Unbind();
		shader->Unbind();
	}

	void Renderer::SubmitModel(const Ref<Shader>& shader, const Ref<Model>& model, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", m_SceneData->viewProjectionMatrix);
		shader->SetMat4("u_Transform", transform);

		model->Draw();

		shader->Unbind();
	}
}
