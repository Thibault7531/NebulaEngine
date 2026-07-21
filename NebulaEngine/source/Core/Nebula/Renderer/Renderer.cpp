#include "Renderer.h"

#include "Renderer2D.h"
#include "gtc/type_ptr.hpp"

namespace Nebula {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData();

	void Renderer::Init()
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("Initializing Nebula Renderer");
		/*
		RenderCommand::Init(api);

		m_SceneData->m_UBO = UniformBuffer::Create({
			{ShaderDataType::Mat4, "u_ViewProjection"},
			{ShaderDataType::Mat4, "u_Transform"}
		});

		Renderer2D::Init();
		*/
	}

	void Renderer::Shutdown()
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("Shutting down Nebula Renderer");
		Renderer2D::Shutdown();

		m_SceneData->m_UBO.reset();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		//RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(const PerspectiveCamera& camera)
	{
		SetRenderState();
		m_SceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
		m_SceneData->m_UBO->SetData(0, glm::value_ptr(m_SceneData->viewProjectionMatrix));
	}

	void Renderer::BeginScene(const OrthographicCamera& camera)
	{
		SetRenderState();
		m_SceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
		m_SceneData->m_UBO->SetData(0, glm::value_ptr(m_SceneData->viewProjectionMatrix));
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(const Ref<Graphics::Shader>& shader, const Ref<Graphics::VertexArray>& vertexArray, const glm::mat4& transform)
	{
		/*
		shader->Bind();
		m_SceneData->m_UBO->SetData(1, glm::value_ptr(transform));
		shader->SetUniformBlock("RendererUBO", m_SceneData->m_UBO);

		vertexArray->Bind();

		RenderCommand::DrawIndexed(vertexArray);

		vertexArray->Unbind();
		shader->Unbind();
		*/
	}

	void Renderer::SubmitModel(const Ref<Graphics::Shader>& shader, const Ref<Model>& model, const glm::mat4& transform)
	{
		/*
		shader->Bind();
		m_SceneData->m_UBO->SetData(1, glm::value_ptr(transform));
		shader->SetUniformBlock("RendererUBO", m_SceneData->m_UBO);

		model->Draw();

		shader->Unbind();
		*/
	}

	void Renderer::SetRenderState()
	{
		/*
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
		*/
	}
}
